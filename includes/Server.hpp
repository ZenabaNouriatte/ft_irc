#pragma once
#include <iostream>
#include <cstdlib> 
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <unistd.h>
#include <vector>



class Server
{
    private :

    int                 _port;
    std::string         _password;
    int                 _server_fd;
    std::vector<int>    _clients_fd;
    int                 _max_fd;

    public :
    Server(int port, const std::string& password);
    ~Server();
    void start();
    void handleError(const std::string& message);
    void acceptNewClient();         
    void handleClient(int fd);     
    void closeClient(int fd);     
    void stop();        
};


