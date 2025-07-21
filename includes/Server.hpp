#pragma once
#include <iostream>
#include <cstdlib> 
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <string.h>
#include <poll.h>
#include <map>
#include <fcntl.h>
#include "Client.hpp"
#include "Message.hpp"
#include <errno.h>
#include <csignal> 

#define BUFFER_SIZE 1024

class Client;

class Server
{
    private :

    int                 _port;
    std::string         _password;
    int                 _server_fd;
    std::vector<pollfd> _poll_fds;
    std::string         _server_name;
    std::map<int, Client*> _clients;

    void handleConsoleInput();


    public :
    Server(int port, const std::string& password);
    ~Server();
    void start();
    void handleError(const std::string& message);      
    void acceptNewClient();
    void handleClient(int client_fd);
    void removeClient(int client_fd);
    void cleanExit();

    static int signal;
    static void catchSignal(int);

    void broadcast(const std::string& text);

    void handleCommand(Client* client, const Message& msg);
    void handlePASS (Client* client, const Message& msg);
    void handleNICK (Client* client, const Message& msg);
    void handleUSER (Client* client, const Message& msg);
    void completeRegistration(Client* client);
    void handleRegistred(Client* client, const Message& msg);
    void handleServerCommand (Client* client, const Message& msg);
    void handlePING (Client* client, const Message& msg);
    void handleMODE (Client* client, const Message& msg);


    void sendError(int fd, const std::string& code, const std::string& target, const std::string& message);


};


