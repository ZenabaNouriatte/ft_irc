#pragma once
#include <iostream>
#include <cstdlib> 
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <string>
#include <poll.h>
#include <map>
#include <fcntl.h>
#include "Client.hpp"
#include "Message.hpp"

#define BUFFER_SIZE 1024

class Client;

class Server
{
    private :

    int                 _port;
    std::string         _password;
    int                 _server_fd;
    std::vector<pollfd> _poll_fds;
    std::map<int, Client*> _clients;



    public :
    Server(int port, const std::string& password);
    ~Server();
    void start();
    void handleError(const std::string& message);      
    void acceptNewClient();
    void handleClient(int client_fd);
    void removeClient(int client_fd);
};


