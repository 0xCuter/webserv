#include "http.hpp"
#include "webserv.hpp"
using namespace std;

Socket::Socket(int port) {
    int opt = true;
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;
    cout << port << endl;
    address.sin_port = htons(port); 
    if ((!(master_socket = socket(AF_INET , SOCK_STREAM , 0)) ) || (setsockopt(master_socket, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0 ))
        throw socket_except();
    if  ((bind(master_socket, (struct sockaddr *)   &address, sizeof(address)) < 0) || (listen(master_socket, MAX_CONNECTION)))
        throw connect_except();
    cout << "Listener on port :" <<  port << endl;
    
}

void Socket::check_ready() {
    int max_sd;

    FD_ZERO(&readfds); FD_ZERO(&writefds);  
    FD_SET(master_socket, &readfds); FD_SET(master_socket, &writefds);  
    max_sd = master_socket;
    for (IT it = c_sd.begin() ; it < c_sd.end() ; it++)  {
        if (*it > 0) 
            FD_SET(*it , &readfds); // FD_SET( *it , &writefds);
        max_sd = max(max_sd, *it);
    }
    if (select( max_sd + 1 , &readfds ,&writefds , NULL , NULL) < 0)
        throw connect_except();
}
    
void Socket::new_connection(const char *message) {
    if (FD_ISSET(master_socket, &readfds))  
    {  
        int addrlen = sizeof(address);
        int new_sd;
        if ((new_sd = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            throw connect_except();
        printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_sd , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
        c_sd.push_back(new_sd);
        fcntl(new_sd, F_SETFL, O_NONBLOCK);
        send(new_sd, message, strlen(message), 0);//can check if full len was sent for errors
    }
}
    
int Socket::messages() {
    int addrlen = sizeof(address);
    for (IT it = c_sd.begin(); it != c_sd.end(); it++) {
        int r;
        if (FD_ISSET(*it , &readfds))  {  
            //incoming message 
            if ((r = read( *it , buffer, BUFF_SIZE)) == 0)  {  
                //Somebody disconnected , get his details and print 
                getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                close(*it);  
                c_sd.erase(it--);
            }  
            else {
                buffer[r] = '\0';  
                // cout << "BUFFER\n"<< buffer << endl;
                // out.open("request.txt"); out << buffer;
                Request req(buffer);
                Response rep(req);
                send(*it, rep.buffer.str().data(), rep.buffer.str().size(), 0);
                    // throw runtime_error("send failed\n");
                getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen); 
                printf("FROM PORT : %d\n%s",ntohs(address.sin_port), buffer);
            }  
        }  
    }  
    return 0;
}
