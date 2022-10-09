#include "http.hpp"

Response::Response(Request &req):version(req.version), status(0), description(""),headers(""), body("") ,req_cp(req) {
    
    int ret = (w.Methods[req.method_name])(req, *this);
    buffer << req.version << " " << ret << " " << w.HttpStatusCode[ret] << "\r\n";
    // if (ret == error_values) get_body and header of error_pages
    buffer << headers << "\r\n";
    buffer << "\r\n" << body.str() << "\r\n";
}

Request::Request(char *buffer) {
    std::string file = string(buffer);
    vector<string> elems(split_set(file.substr(0,file.find("\r\n")), " "));
    //check there are 3 parts
    method_name = elems[0];url = elems[1];version = elems[2];
    url = (url == "/") ? w.home : w.root + url;
    header = file.substr(file.find("\r\n") + 1,file.find("\r\n\r\n"));
    body = file.substr(file.find("\r\n\r\n"));
}

Request::~Request() {;}
Response::~Response() {;}

int GET(Request &req, Response &rep) {

        ifstream file_stream;
    file_stream.open(req.url.data());
    if (file_stream.fail()) 
        return(404);
    std::string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
    rep.body << file;
    file_stream.close();
    return (200);
}

//   {401 ,"Unauthorized"},
//   {403 ,"Forbidden"},
int DELETE(Request &req, Response &rep) {
    (void) rep;
    if (remove(req.url.data()))
        return(404);
    return (200);
}

//either create file or append content to existing file
int POST(Request &req, Response &rep) {
    (void) rep;
    ofstream new_file;
    new_file.open((string(req.url)).data());
    if (new_file.fail())
        return 401;
    new_file << req.body;
    return 201;
}
