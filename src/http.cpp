#include "http.hpp"
#include <wait.h>
#include <dirent.h>

typedef std::map<std::string, std::vector<std::string> >::iterator M;

Response::Response(const Request& req) :
	w(req.w),
	version(req.version),
	body_size(0),
	max_body_size(req.host->max_body_size),
	method(req.method_name),
	route_methods(req.host->route_methods),
	root(req.host->root),
	url(req.url),
	content_type("text/plain"),
	req_cp(req)
{
	int ret = exec_method();
	check_error_code(ret);
	set_header(ret);
	full_content.assign(header.begin(), header.end());
	full_content.insert(full_content.end(), body.begin(), body.end());
}

Response::~Response() {;}

int Response::exec_method(void)
{
	M m;

	if (((m = route_methods.find(url)) != route_methods.end()
			&& find(m->second.begin(), m->second.end(), method) == m->second.end()))
		return 405;
	else if (w.Methods.find(method) == w.Methods.end())
		return 400;
	else
		return (w.Methods[method])(*this);
}

// If the status code represents an error displays relevant error page
void Response::check_error_code(int code)
{
	stringstream body_stream;

	if (code >= 400)
	{
		ifstream file_stream;
		file_stream.open((root + "/" + req_cp.host->error_path + to_string(code) + ".html").c_str());
		if (file_stream.fail())
			file_stream.open((root + "/" + w.error_path + to_string(code) + ".html").c_str());
		body = vector<char>(istreambuf_iterator<char>(file_stream), istreambuf_iterator<char>());
		file_stream.close();
		body_size = body.size();

		// // Else generic code
		// body.clear();
		// body_stream << "<h1>Error code: " << code << " | "
		// 			<< w.HttpStatusCode[code] << "<h1>\r\n";
		// string b = body_stream.str();
		// body.assign(b.begin(), b.end());
	}
}

// Checks extension type and assigns content_type accordingly for the response
void Response::set_content_type()
{
	if (content_type == "text/html")
		return;
	size_t ext_pos = url.find_last_of(".");
	content_type = "text/plain";
	if (ext_pos != string::npos)
	{
		string path_extension = url.substr(ext_pos + 1);
		if (path_extension == "html")
			content_type = "text/html";
		else if (path_extension == "css")
			content_type = "text/css";
		else if (path_extension == "js")
			content_type = "text/javascript";
		else if (path_extension == "md")
			content_type = "text/markdown";
		else if (path_extension == "jpg" || path_extension == "jpeg")
			content_type = "image/jpeg";
		else if (path_extension == "png")
			content_type = "image/png";
		else if (path_extension == "gif")
			content_type = "image/gif";
		else if (path_extension == "ico")
			content_type = "image/x-icon";
		else if (path_extension == "mp4")
			content_type = "video/mp4";
	}
}

// Generates header html
void Response::set_header(int code)
{
	stringstream header_stream;

	set_content_type();
	header_stream << "HTTP/1.1 " << code << " " <<  w.HttpStatusCode[code] << "\r\n"
				  << "Access-Control-Allow-Origin: *\r\n";
	header_stream << "Host: " << req_cp.host->names[0] << "\r\n";
	header_stream << "Content-Length: " << body_size << "\r\n";
	header_stream << "Content-Type: " << content_type << "\r\n";
	header_stream << "Accept-Ranges: bytes" << "\r\n";
	header_stream << "Server: DeepThought/42" << "\r\n";
	if (url.find("cookie"))
		header_stream <<  "Set-Cookie: cookie=cookie" << "\r\n";
	if (req_cp.header.find("Cookie: "))
		cout << "user on sd " <<req_cp.sd << " has a cookie" << endl;
	header_stream << "\r\n";
	header = header_stream.str();
}

int autoindex(Response &rep)
{
	// Checks for possible index files referenced in the config (Looks like it's already done in request)
	// for (vector<string>::iterator it = config_.index.begin(); it != config_.index.end(); it++)
	// {
	// 	requested_file.open((url + *it).c_str());
	// 	if (requested_file.is_open())
	// 	{
	// 		url += *it;
	// 		return 0;
	// 	}
	// }

	// Get list of files in directory and adds it to the html body
	// See https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
	// if (config_.autoindex == true && !requested_file.is_open())
	// {
	rep.content_type = "text/html";
	stringstream body_stream;
	body_stream << "<!DOCTYPE html>\r\n";
	body_stream << "<h1>Files in directory</h1>\r\n" << "<ul>\r\n";
	// Directory stream
	DIR *dir;
	struct dirent *current;
	if ((dir = opendir(rep.url.c_str())) != NULL)
	{
		// Cicles trhough all the files in the directory
		while ((current = readdir(dir)) != NULL)
		{
			// 'current->d_name' name of the file
			body_stream << "<li><a href=\"" << rep.url + current->d_name << "\">"
						<< current->d_name << "</a></li>" << "\r\n";
		}
		closedir(dir);
	}
	body_stream << "</ul>\r\n";
	body_stream << "\r\n";
	string body_str = body_stream.str();
	rep.body.assign(body_str.begin(), body_str.end());
	rep.body_size = rep.body.size();
	if (rep.body_size > rep.max_body_size)
		return 413;
	return 200;
	// }
}

// Generates response body html based on the request
int GET(Response &rep)
{
	if (rep.url.find(rep.root + "/cgi-bin/") != string::npos)
		return CGI(rep);

	ifstream requested_file;
	bool image_type = false;

	rep.body.clear();
	// Check if path is accessible, if the file exists and if it's readable
	if (access(rep.url.c_str(), F_OK | R_OK) != 0)
		return 404;
	if (*(rep.url.end() - 1) == '/')
		return autoindex(rep);
	// Get file_type and open file
	size_t ext_pos = rep.url.find_last_of(".");
	if (ext_pos != string::npos)
	{
		string path_extension = rep.url.substr(ext_pos + 1);
		if (path_extension == "jpeg" || path_extension == "jpg" || path_extension == "png" || path_extension == "gif")
			image_type = true;
	}
	if (image_type)
		// https://stackoverflow.com/a/61140915/14409847
		requested_file.open(rep.url.c_str(), ios_base::in | ios_base::binary);
	else
		requested_file.open(rep.url.c_str());

	if (!requested_file.is_open())
		return 404;
	else
	{
		// Fill body with file content
		rep.body = vector<char>(istreambuf_iterator<char>(requested_file), istreambuf_iterator<char>());
		requested_file.close();
		rep.body_size = rep.body.size();
		if (rep.body_size > rep.max_body_size)
			return 413;
		return 200;
	}
}

//either create file or append content to existing file
int POST(Response &rep)
{
	ofstream new_file;
	new_file.open(rep.url.c_str());
	if (new_file.fail())
		return 401;
	new_file << rep.req_cp.body;
	string b = "<h1>File '" + rep.url + "' has been posted.</h1>\r\n";
	rep.body.assign(b.begin(), b.end());
	rep.body_size = rep.body.size();
	rep.content_type = "text/html";
	return 201;
}

// Removes file and adds html body, if there is no file to remove 'Error 404'
int DELETE(Response &rep)
{
	if (remove(rep.url.c_str()) != 0)
		return 404;
	string b = "<h1>File '" + rep.url + "' has been deleted.</h1>\r\n";
	rep.body.assign(b.begin(), b.end());
	rep.body_size = rep.body.size();
	rep.content_type = "text/html";
	return 200;
}

int CGI(Response &rep)
{
	char *av[3];
	int pip[2];
	int ret,r;
	char buffer[2048];
	string cgi = "";
	stringstream body_stream;

	if (pipe(pip) != 0)
		return 500;
	for (vector<string>::iterator it = rep.req_cp.host->cgi_ext.begin(); it != rep.req_cp.host->cgi_ext.end(); it++)
		if (rep.url.find(*it) == rep.url.size() - it->size())
			cgi = *it + "-cgi";
	if (cgi.empty())
		return 403;
	cout << cgi << endl;
	if (access(("/" + rep.w.cwd + "/" + rep.url).c_str(), X_OK) < 0)
		return 401;
	pid_t pid = fork();
	if (pid < 0) return 502;
	if (!pid) {
		av[0] = strdup((char *)("/" + rep.w.cwd + "/" + rep.root + "/cgi-bin/" + cgi).c_str());
		av[1] = strdup((char *)("/" + rep.w.cwd + "/" + rep.url).c_str());
		av[2] = NULL;
		// if (req_cp.body.size() && req_cp.body.find_first_not_of("     \n\r") != string::npos)
		//     av[1] = strdup(req_cp.body.c_str());
		if (!av[0] || !av[1])
			return 500;
		cerr << av[0] << "||" << av[1]  << endl;
		close(pip[0]);
		dup2(pip[1],1);
		if (execve(av[0], av, rep.w.env) == -1)
			perror("exec failed");
		exit(1);
	}
	close(pip[1]);
	if (waitpid(pid, &ret, 0) == -1)
		return 500;
	body_stream << "<body>\n";
	while ((r = read(pip[0], buffer, 2048)) != 0){
		buffer[r] = 0; cgi = string(buffer); replace(cgi, "\n", "<br>");
		body_stream << cgi;}
	body_stream << "</body>";
	string body_str = body_stream.str();
	rep.body.assign(body_str.begin(), body_str.end());
	rep.body_size = rep.body.size();
	rep.content_type = "text/html";
	if (WIFEXITED(ret) & WEXITSTATUS(ret))
		return 502;
	return 200;
}



Request::Request(char *buffer, WebServ *web, int sd, int port): w(*web), sd(sd), obuff(buffer), port(port){
    string file = string(buffer);
    vector<string> elems(split_set(file.substr(0,file.find("\r\n")), " "));
    //if (!get_val("Content-length").empty())https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Transfer-Encoding
    if (elems.size() != 3) throw invalid_argument("invalid request");
    method_name = elems[0];url = elems[1];version = elems[2];
    header = file.substr(file.find("\r\n") + 1,file.find("\r\n\r\n"));
    body = file.substr(file.find("\r\n\r\n"));
    host = w.get_host(get_val("Host")[0], port);

    if (url.find_last_of("/") == url.size() - 1) {
        for (map<string, string>::iterator i = host->dirs.begin(); i != host->dirs.end(); i++)
            if (cout << i->first << endl && i->first == url)
                url = w.root + "/" + i->second;
        if (url == elems[1] && url == "/")
            url = w.root + "/" + w.home;
    } else if (url.find(".html") != string::npos && url.find("/HTML") == string::npos) {
            url = w.root + "/" + "HTML" + url;
    } else { url = w.root + url;}

}

vector<string> Request::get_val(string key) {//to get host
    string content;
    size_t start = header.find(key);
    if (start != string::npos)
        start = header.find_first_of(":", start);
    if (header.find_first_not_of(": ", start) != string::npos && header.find_first_of("\n", start) != string::npos)
        content = header.substr(header.find_first_not_of(":", start)  ,header.find_first_of("\n",start) - header.find_first_not_of(": ", start));
    return split_set(content, ";");
}

Request::~Request() {;}



// typedef std::map<std::string, std::vector<std::string> >::iterator M;
// Response::Response(const Request &req): w(req.w),version(req.version), status(0), description(""),headers(""), body("") ,req_cp(req) {
//     M m;
//     int ret;
//     if (((m = req.host->route_methods.find(req.url))!= req.host->route_methods.end()
//             && find(m->second.begin(), m->second.end(),req.method_name) == m->second.end()))
//         ret = 405;
//     else if (w.Methods.find(req.method_name) == w.Methods.end())
//         ret = 400;
//     else
//         ret = (w.Methods[req.method_name])(req_cp, *this);
//     buffer << req.version << " " << ret << " " << w.HttpStatusCode[ret] << "\r\n";
//     if (ret < 400) { //body gets set by method
//         headers += "Content-length: " + to_string(body.str().size()) + "\n";
//         if (req.url.find("cookie"))
//             headers +=  "Set-Cookie: cookie=cookie\n";
//         if (req.header.find("Cookie: "))
//             cout << "user on sd " <<req.sd << " has a cookie" << endl;
//         buffer << headers << "\r\n";
//         buffer << "\r\n" << body.str() << "\r\n";\
//     } else {
//         ifstream file_stream;
//         file_stream.open((w.root + "/" + req_cp.host->error_path + to_string(ret) + ".html").data());
//         if (file_stream.fail())
//             file_stream.open((w.root + "/" + w.error_path + to_string(ret) + ".html").data());
//         string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
//         body << file;
//         headers += "Content-length: " + to_string(body.str().size()) + "\n";
//         buffer << headers << "\r\n";
//         buffer << "\r\n" << body.str() << "\r\n";
//     }
// }

// Response::~Response() {;}

// int GET(Request &req, Response &rep) {
//     if (req.url.find(req.w.root + "/cgi-bin/") != string::npos)
//         return CGI(req, rep);
//     ifstream file_stream;
//     file_stream.open(req.url.data());
//     if (file_stream.fail())
//         return(404);
//     std::string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
//     rep.body << file;//check if css file and insert in header
//     return (200);
// }

// //   {401 ,"Unauthorized"},
// //   {403 ,"Forbidden"},
// int DELETE(Request &req, Response &rep) {
//     (void) rep;
//     if (remove((req.w.root + req.url).data()))
//         return(404);
//     return (200);
// }

// //either create file or append content to existing file
// int POST(Request &req, Response &rep) {
//     (void) rep;
//     ofstream new_file;
//     new_file.open((string(req.url)).data());
//     if (new_file.fail())
//         return 401;
//     new_file << req.body;
//     return 201;
// }

// int CGI(Request &req, Response &rep) {
//     char *av[3];
//     int pip[2];
//     int ret,r;
//     char buffer[2048];
//     string cgi = "";

//     if (pipe(pip) != 0)
//         return 500;
//     for (vector<string>::iterator it = req.host->cgi_ext.begin(); it != req.host->cgi_ext.end(); it++)
//         if (req.url.find(*it) == req.url.size() - it->size())
//             cgi = *it + "-cgi";
//     if (cgi.empty())
//         return 403;
//     cout << cgi << endl;
//     if (access(("/" + req.w.cwd + "/" + req.url).data(), X_OK) < 0)
//         return 401;
//     pid_t pid = fork();
//     if (pid < 0) return 502;
//     if (!pid) {
//         av[0] = strdup((char *)("/" + req.w.cwd + "/" + req.w.root + "/cgi-bin/" + cgi).c_str());
//         av[1] = strdup((char *)("/" + req.w.cwd + "/" + req.url).data());
//         av[2] = NULL;
//         // if (req.body.size() && req.body.find_first_not_of("     \n\r") != string::npos)
//         //     av[1] = strdup(req.body.data());
//         if (!av[0] || !av[1])
//             return 500;
//         cerr << av[0] << "||" << av[1]  << endl;
//         close(pip[0]);
//         dup2(pip[1],1);
//         if (execve(av[0], av, req.w.env) == -1)
//             perror("exec failed");
//         exit(1);
//     }
//     close(pip[1]);
//     if (waitpid(pid, &ret, 0) == -1)
//         return 500;
//     rep.body << "<body>\n";
//     while ((r = read(pip[0], buffer, 2048)) != 0){
//         buffer[r] = 0; cgi = string(buffer); replace(cgi, "\n", "<br>");
//         rep.body << cgi;}
//     rep.body << "</body>";
//     if (WIFEXITED(ret) & WEXITSTATUS(ret))
//         return 502;
//     return 200;
// }
