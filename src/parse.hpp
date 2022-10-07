#pragma once
#include "webserv.hpp"
using namespace std;
typedef vector<string>::iterator IT;
typedef vector<vector<string> >::iterator IT_2;

vector<string> split_str(std::string s, std::string delimiter);
vector<string> split_set(std::string s, std::string delimiter);
vector<vector<string> > parse_config(std::string conf_path);