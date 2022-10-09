#pragma once
#include "webserv.hpp"
using namespace std;

class Server
{

private:
    typedef vector<Socket>::iterator IT;
    // int             c_max_size;
    vector<int>     ports;
    vector<string>  names;
    std::string     error_path;
    int             max_body_size;
    vector<string>  methods;
    vector<string>  cgi_ext;
    //
    vector<Socket>  sockets;
    vector<vector<string> > settings;
    //

public:
    Server(std::string config_path) {
        try {settings = parse_config(config_path);} 
        catch (exception &e){cerr << e.what() << std::endl;}
        for (size_t i = 0; i < settings.size(); i++) {
            if (settings[i][0] == "listen") {
                if (settings[i].size() < 2) ports.push_back(80);//default
                for (size_t j = 1; j < settings[i].size(); j++)
                    ports.push_back(strtol(settings[i][j].data(),NULL,10)); 
            }
            else if (settings[i][0] == "server_name")
                names = vector<string>(settings[i].begin() + 1, settings[i].end());
            else if (settings[i][0] == "methods")
                methods = vector<string>(settings[i].begin() + 1, settings[i].end());
            else if (settings[i][0] == "cgi")
                cgi_ext = vector<string>(settings[i].begin() + 1, settings[i].end());
            else if (settings[i][0] == "max_body_size")
                max_body_size = strtol(settings[i][1].data(),NULL,10);
            else if (settings[i][0] == "errors")
                error_path = settings[i][1];
        }
        for (size_t i = 0; i < ports.size(); i++)
            sockets.push_back(Socket(ports[i]));
        ifstream file_stream;
        file_stream.open("HTML/wow.html");
        string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
    }

    int run() {
        while(true)  {
             for (IT it = sockets.begin(); it != sockets.end(); it++) {
                it->check_ready();
                it->new_connection();//change empty response to homepage or somthionig
                it->messages();
            }
        }  
    }

};

// int CGI::execute() {
//   file_path_ = cwd_ + "/" + file_.getPath();

//   if (!setCGIEnv())
//     return 500;
//   if (!(argv_[0] = ft::strdup(cgi_path_.c_str())))
//     return 500;
//   if (!(argv_[1] = ft::strdup(file_path_.c_str())))
//     return 500;
//   argv_[2] = NULL;

//   int pip[2];

//   if (pipe(pip) != 0)
//     return 500;

//   pid_t pid = fork();

//   if (pid == 0) {
//     if (chdir(file_path_.substr(0, file_path_.find_last_of('/')).c_str()) == -1)
//       return 500;
//     close(pip[1]);
//     if (dup2(pip[0], 0) == -1)
//       return 500;
//     if (dup2(tmp_file_.getFd(), 1) == -1)
//       return 500;
//     close(pip[0]);
//     execve(argv_[0], argv_, env_);
//     exit(1);
//   }
//   else if (pid > 0) {
//     close(pip[0]);
//     if (req_body_.length() && write(pip[1], req_body_.c_str(), req_body_.length()) <= 0)
//       return 500;
//     close(pip[1]);

//     int status;

//     if (waitpid(pid, &status, 0) == -1)
//       return 500;
//     if (WIFEXITED(status) && WEXITSTATUS(status))
//       return 502;
//   }
//   else {
//     return 502;
//   }

//   body_ = tmp_file_.getContent();
//   return 200;
// }
