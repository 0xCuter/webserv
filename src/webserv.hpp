#pragma once
#define MAX_CONNECTION 128
#define TIMEOUT 5000
#define BUFF_SIZE 4096
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
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/select.h>
# include <signal.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <iostream>
#include <ostream>
# include <map>
#include <set>
//~~~~~~~~~~~~~~~~~~~~//

class Response;
class Request;
typedef int (*Method) (Request &, Response &);
using namespace std;

class WebServ {
public:
    string root;
    string home;
    map<string, Method> Methods;
    map<int, string> HttpStatusCode;
    set<string> cgi_exts;
    WebServ();
    ~WebServ();
};
# include "socket.hpp"
# include "parse.hpp"
# include "http.hpp"

