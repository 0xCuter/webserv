#include <stdio.h> 
#include <cstring>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <vector>    
#include <algorithm>
#include <locale>   
#define PORT 8080
#define MAX_CONNECTION 128
using namespace std;
typedef vector<int>::iterator IT;
#define Buff_size 4096 //size of http message

int main(int ac , char *av[])  
{  
    (void)av;
    (void)ac;
    int opt = true;  
    int master_socket , addrlen = 0 , new_socket , valread;  
    int max_sd;  
    vector<int> c_socks;
    struct sockaddr_in address;  
         
    char buffer[Buff_size];  //data buffer portof 1K 

    //set of socket descriptors 
    fd_set readfds;  
    fd_set writefds;  
         
    //a message 
    const char *message = "HTTP/1.1 200 OK\r\n"  "<!doctype html>\n<html>\n  <head>\n    <title>This is the title of the webpage!</title>\n  </head>\n  This is an example paragraph. Anything in the <strong>body</strong> tag will appear on the page, just like this <strong>p</strong> tag and its contents.\n    <a href=\"./cgi_bash.sh\"> \nlink!\n</a>\n  </body>\n</html>\n";
     
    //initialise all client_socket[] to 0 so not checked 
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
     
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )    {  
        perror("setsockopt");
        exit(EXIT_FAILURE);  
    }  
     
    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {perror("bind failed");
        exit(EXIT_FAILURE);}
    else
        printf("Listener on port %d \n", PORT);
         
    //try to specify maximum of MAX_CONNECTION pending connections for the master socket 
    if (listen(master_socket, MAX_CONNECTION) < 0) {perror("listen");  
        exit(EXIT_FAILURE);}
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  
    while(true)  
    {
        //clear the socket set 
        FD_ZERO(&readfds); FD_ZERO(&writefds);  
        //add master socket to set 
        FD_SET(master_socket, &readfds); FD_SET(master_socket, &writefds);  
        //add child sockets to set 
        max_sd = master_socket;
        for (IT it = c_socks.begin() ; it < c_socks.end() ; it++)   
            if(*it > 0)  //if valid socket descriptor then add to read list
                {FD_SET( *it , &readfds); // FD_SET( *it , &writefds);
                max_sd = max(max_sd, *it);} //highest file descriptor number, need it for the select function 

        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        if ( select( max_sd + 1 , &readfds ,&writefds , NULL , NULL) < 0)
            printf("select error");  
             
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {perror("accept");  
                exit(EXIT_FAILURE);}
             
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
           
            //send new connection greeting message 
            if( send(new_socket, message, strlen(message), 0) != (ssize_t)strlen(message)) 
                perror("send");
                 
            puts("Welcome message sent successfully");  
                 
            //add new socket to array of sockets 
            c_socks.push_back(new_socket);
            printf("Adding to list of sockets as %d\n" , new_socket);  
        }
        //else its some IO operation on some other socket
        for (IT it = c_socks.begin(); it != c_socks.end(); it++)  {  
            if (FD_ISSET(*it , &readfds))  {  
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( *it , buffer, Buff_size)) == 0)  {  
                    //Somebody disconnected , get his details and print 
                    getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                    //Close the socket and mark as 0 in list for reuse 
                    close(*it);  
                    c_socks.erase(it--);

                }  
                //Echo back the message that came in 
                else {  
                    //set the std::stringterminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                    getpeername(*it , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    printf("FROM PORT : %d\n%s",ntohs(address.sin_port), buffer);
                }  

            }  
        }  
    }  
         
    return 0;  
}  