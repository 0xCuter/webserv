#include "webserv.hpp"
#include "server.hpp"
#include <iostream>
using namespace std;

int main(int ac, char **av) {
    if (ac < 2) {cerr << "require a single configuration file\n"; return 1;}
    try {
        Server serv(av[1]);
        serv.run();
    } catch (exception &e) {cerr << e.what() << endl;}
    
    return 0;
}