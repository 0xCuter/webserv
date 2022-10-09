#include "webserv.hpp"

WebServ::~WebServ() {;}

WebServ::WebServ() {
      root = "HTML";
      home = "HTML/wow.html";
      cgi_exts.insert(".php");
      cgi_exts.insert(".py");
      
      Methods.insert(make_pair("GET",    &GET));
      Methods.insert(make_pair("POST",   &POST));
      Methods.insert(make_pair("DELETE", &DELETE));

      HttpStatusCode.insert(make_pair(100 ,"Continue"));
      HttpStatusCode.insert(make_pair(101 ,"SwitchingProtocols"));
      HttpStatusCode.insert(make_pair(103 ,"EarlyHints"));
      HttpStatusCode.insert(make_pair(200 ,"Ok"));
      HttpStatusCode.insert(make_pair(201 ,"Created"));
      HttpStatusCode.insert(make_pair(202 ,"Accepted"));
      HttpStatusCode.insert(make_pair(203 ,"NonAuthoritativeInformation"));
      HttpStatusCode.insert(make_pair(204 ,"NoContent"));
      HttpStatusCode.insert(make_pair(205 ,"ResetContent"));
      HttpStatusCode.insert(make_pair(206 ,"PartialContent"));
      HttpStatusCode.insert(make_pair(300 ,"MultipleChoices"));
      HttpStatusCode.insert(make_pair(301 ,"MovedPermanently"));
      HttpStatusCode.insert(make_pair(302 ,"Found"));
      HttpStatusCode.insert(make_pair(304 ,"NotModified"));
      HttpStatusCode.insert(make_pair(400 ,"BadRequest"));
      HttpStatusCode.insert(make_pair(401 ,"Unauthorized"));
      HttpStatusCode.insert(make_pair(403 ,"Forbidden"));
      HttpStatusCode.insert(make_pair(404 ,"NotFound"));
      HttpStatusCode.insert(make_pair(405 ,"MethodNotAllowed"));
      HttpStatusCode.insert(make_pair(408 ,"RequestTimeout"));
      HttpStatusCode.insert(make_pair(418 ,"ImATeapot"));
      HttpStatusCode.insert(make_pair(500 ,"InternalServerError"));
      HttpStatusCode.insert(make_pair(501 ,"NotImplemented"));
      HttpStatusCode.insert(make_pair(502 ,"BadGateway"));
      HttpStatusCode.insert(make_pair(503 ,"ServiceUnvailable"));
      HttpStatusCode.insert(make_pair(504 ,"GatewayTimeout"));
      HttpStatusCode.insert(make_pair(505 ,"HttpVersionNotSupported"));
}