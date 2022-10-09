#pragma once
#include "webserv.hpp"
using namespace std;

class Request {
public:
    WebServ w;//used for http code lookup and method implementation
    string version; //start line
    string method_name; 
    string url;
    string header;
    string body;
    // Header field
    Request(char *buffer);
    ~Request();
};

class Response{
public:
    WebServ w;//used for http code lookup and method implementation
    string version;
    int     status;
    string description;
    //start line
    string headers;
    //empty line
    stringstream body;   
    stringstream buffer;
    Request req_cp; 
    Response(Request &req);
    ~Response();
};


int GET(Request &req, Response &rep);
int DELETE(Request &req, Response &rep);
int POST(Request &req, Response &rep);
