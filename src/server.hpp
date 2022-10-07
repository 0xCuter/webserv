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
    string          home;

public:
    Server(std::string config_path) {
        try {
            settings = parse_config(config_path);
        } catch (exception &e){
            cerr << e.what() << std::endl;
        }
        for (size_t i = 0; i < settings.size(); i++) {
            if (settings[i][0] == "listen") {
                if (settings[i].size() < 2) ports.push_back(80);//default
                for (size_t j = 1; j < settings[i].size(); j++)
                    ports.push_back(strtol(settings[i][j].data(),NULL,10)); 
            }
            else if (settings[i][0] == "server_name")
                names = vector<string>(settings[i].begin() + 1, settings[i].end());
            else if (settings[i][0] == "errors")
                error_path = settings[i][1];
            else if (settings[i][0] == "max_body_size")
                max_body_size = strtol(settings[i][1].data(),NULL,10);
            else if (settings[i][0] == "methods")
                methods = vector<string>(settings[i].begin() + 1, settings[i].end());
            else if (settings[i][0] == "cgi")
                cgi_ext = vector<string>(settings[i].begin() + 1, settings[i].end());
        }
        for (size_t i = 0; i < ports.size(); i++)
            sockets.push_back(Socket(ports[i]));
        ifstream file_stream;
        file_stream.open("HTML/wow.html");
        string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
        home = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n" + file;
    }

    int run() {

        while(true)  {
             for (IT it = sockets.begin(); it != sockets.end(); it++) {
                it->check_ready();
                it->new_connection(home.data());
                it->messages();
            }
        }  
    }

};
