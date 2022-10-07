#pragma once
#include "http.hpp"
#include "webserv.hpp"
using namespace std;

class Socket {
private:
    typedef vector<int>::iterator IT;
    std::string     name;
    vector<string>  methods;
    int             master_socket;
    fd_set          readfds;  
    fd_set          writefds;  
    vector<int>     c_sd;
    struct sockaddr_in address;
    char            buffer[BUFF_SIZE];
    class socket_except :  public exception {
    public:
        const char* what() const throw() {
            return ("Couldn't initialize socket\n");
        }
    };

    class connect_except : public exception {
    public:
        const char* what() const throw() {
            return ("Couldn't connect to client socket\n");
        }
    };
    
public:
    Socket(int port) ;

    void check_ready();
    
    void new_connection(const char *message) ;
    
    int messages();


};

