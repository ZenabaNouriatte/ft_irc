#pragma once
#include <iostream>
#include <cstdlib> 
#include <sys/socket.h>
#include <stdexcept>
#include <netinet/in.h>


class Server
{
    private :

    int             _port;
    std::string     _password;
    int             _server_fd;

    public :
    Server(int port, const std::string& password);
    ~Server();
    void start();
};


