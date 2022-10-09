#include "parse.hpp"

typedef vector<string>::iterator IT;
typedef vector<vector<string> >::iterator IT_2;

vector<string> split_str(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }
    res.push_back (s.substr (pos_start));
    return res;
}

vector<string> split_set(std::string s, std::string delimiter) {
    vector<string> ret;
    size_t pos_start = 0, pos_end = 0;
    std::string token;

    while ((pos_start = s.find_first_not_of(delimiter, pos_end)) != string::npos) {
        pos_start = s.find_first_not_of(delimiter, pos_end);
        pos_end = s.find_first_of(delimiter, pos_start);
        ret.push_back(s.substr(pos_start, pos_end - pos_start));
    }
    return ret;
}

vector<vector<string> > parse_config(std::string conf_path) {
    vector<vector<string> > res;
    string params[] = {"listen" ,"server_name" ,"errors" ,"max_body_size","methods" ,"cgi"};
    ifstream file_stream;
    file_stream.open(conf_path.data());
    if (!file_stream)
        throw invalid_argument("file doesnt exist\n");
    std::string file((istreambuf_iterator<char>(file_stream)), istreambuf_iterator<char>());
    file_stream.close();

    vector<string> lines = split_set(file, "\n");
    for (IT it = lines.begin(); it < lines.end(); it++)
        res.push_back(split_set(*it, "  ="));
    
    int is;
    for (IT_2 it2 = res.begin(); it2 < res.end(); it2++) {
        is = 0;
        for (int i = 0; i < 6; i++)
            if ((*it2).size())
                is += (params[i] == (*it2)[0]);
        if (is != 1)//check if valid name
            throw exception();//wrong number of params either repeated or not present
        // else {
        //     if ((*it2)[0] == "errors") //no more than one arg & check if exists
        //     else if ((*it2)[0] == "max_body_size") // check if positive
        //     else if ((*it2)[0] == "methods") // check if subset of existing methods 
        //     else if ((*it2)[0] == "cgi") //check if subset of existing extensions
        // }
    }
    return res;
}