#pragma once
#include "webserv.hpp"
#include "server.hpp"
using namespace std;

class Request {
public:
    WebServ &w;//used for http code lookup and method implementation
    string version;
    string method_name;
    string url;
    string header;
    string body;
    Server *host;
    int     sd;
    string obuff;
    int     port;
    Request(char *buffer, WebServ *web, int sd, int port);
    ~Request();
    vector <string> get_val(string key);
    Request &operator=(const Request &req) {
        w = req.w;
        version = req.version;
        method_name = req.method_name;
        url = req.url;
        header = req.header;
        body = req.body;
        host = req.host;
        sd = req.sd;
        return *this;
    }
};

class Response
{
	public:

		WebServ &w;
		string version;
		size_t body_size;
		size_t max_body_size;
		string method;
		map<string, vector<string> > route_methods;
		string root;
		string url;
		string content_type;
		string server;
		Request req_cp;

		string header;
		vector<char> body;
		vector<char> full_content;

		int exec_method(void);
		void check_error_code(int code);
		void set_content_type(void);
		void set_header(int code);

		Response(const Request& request);
		~Response();

		Response(const Response &rep): w(rep.w), req_cp((char *)rep.req_cp.obuff.data(), &rep.req_cp.w, (const int)rep.req_cp.sd, rep.req_cp.port)
		{
			version = rep.version;
			body_size = rep.body_size;
			max_body_size = rep.max_body_size;
			method = rep.method;
			route_methods = rep.route_methods;
			root = rep.root;
			url = rep.url;
			content_type = rep.content_type;
			server = rep.server;
			header = rep.header;
			body = rep.body;
			full_content = rep.full_content;
		}

		Response &operator=(const Response &rep)
		{
			w = rep.w;
			version = rep.version;
			body_size = rep.body_size;
			max_body_size = rep.max_body_size;
			method = rep.method;
			route_methods = rep.route_methods;
			root = rep.root;
			url = rep.url;
			content_type = rep.content_type;
			server = rep.server;
			header = rep.header;
			body = rep.body;
			full_content = rep.full_content;
			req_cp = rep.req_cp;
			return *this;
		}
};

int GET(Response &rep);
int DELETE(Response &rep);
int POST(Response &rep);
int CGI(Response &rep);
int autoindex(Response &rep);

// class Response{
// public:
//     WebServ &w;//used for http code lookup and method implementation
//     string version;
//     int     status;
//     string description;
//     //start line
//     string headers;
//     //empty line
//     stringstream body;
//     stringstream buffer;
//     Response(const Request &req);
//     Response(const Response &rep): w(rep.w), req_cp((char *)rep.req_cp.obuff.data(), &rep.req_cp.w, (const int)rep.req_cp.sd, rep.req_cp.port) {
//         version = rep.version;
//         status = rep.status;
//         description = rep.description;
//         headers = rep.headers;
//         body << rep.body.str();
//         buffer << rep.buffer.str();
//     }
//     ~Response();
//     Request req_cp;
//     Response &operator=(const Response &rep) {
//         w = rep.w;
//         version = rep.version;
//         status = rep.status;
//         description = rep.description;
//         headers = rep.headers;
//         body << rep.body.str();
//         buffer << rep.buffer.str();
//         req_cp = rep.req_cp;
//         return *this;
//     }

// };




// int GET(Request &req, Response &rep);
// int DELETE(Request &req, Response &rep);
// int POST(Request &req, Response &rep);
// int CGI(Request &req, Response &rep);
