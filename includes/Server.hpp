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
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include "Channel.hpp"
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <poll.h>
#include <sstream>
#include <string.h>
#include <string>
#include <sys/socket.h> // socket, bind, listen, accept
#include <unistd.h>
#include <vector>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"

#define BUFFER_SIZE 1024

class	Client;

class Server
{
  private:

    int                     _port;
    std::string             _password;
    int                     _server_fd;
    std::vector<pollfd>     _poll_fds;
    std::string             _server_name;
    std::map<int, Client*>  _clients;
    std::vector<Channel>    _channels;

    public :

    /*===== Constructors / Destructor =====*/
    Server(int port, const std::string& password);
    ~Server();

    /*===== Startup / Signal Handling / Error =====*/
    void start();                                       // Main loop
    static void catchSignal(int);                       // Handle Ctrl+C or kill signals
    void handleError(const std::string& message);       // Print error and close server socket
    void cleanExit();                                   // Cleanup sockets and memory
   
    /*===== Server Setup =====*/
    bool setupServerSocket();                           // Create, bind, listen on socket
    void setupPollFds();                                // Add server socket and stdin to poll()
    void printStart();                                  // Display server start message
    void handlePollEvents();                            // Process events returned by poll()

    /*===== Client Connection Management =====*/
    void acceptNewClient();                             // Accept new connection and register client
    void removeClient(int client_fd);                   // Remove client (socket, pollfd, memory)




    /*===== Client Communication =====*/
    void handleClient(int client_fd);                                                           // Read data from client
    void handleClientRead(Client* client, const std::string& input);                            // Handle full commands
    void handleClientDisconnection(Client* client, int client_fd, ssize_t received_bytes);      // Manage disconnect or error

    
    /*===== IRC Command Handling =====*/
    void handleCommand(Client* client, const Message& msg);
    void completeRegistration(Client* client);
    void handleRegistred(Client* client, const Message& msg);
    void handleServerCommand (Client* client, const Message& msg);
    
    void handlePASS (Client* client, const Message& msg);
    void handleNICK (Client* client, const Message& msg);
    void handleUSER (Client* client, const Message& msg);
    void handlePRIVMSG(Client* client, const Message& msg);
    void handlePING (Client* client, const Message& msg);
    void handleMODE (Client* client, const Message& msg);
    void handleJOIN (Client* client, const Message& msg);
    bool parseJoin(const Message &msg, std::string &channel, std::string &key);
	  bool PrefixUser(const Message &msg, std::string &User,
	  std::string &channel, std::string &key);
    std::string userPrefix(const std::string& prefix);
    Channel* findChannel(const std::string& name);
    std::vector<std::string> splitComma(const std::string &input);
    void handleSingleJoin(Client *client, const std::string &channelName, const std::string &key);
    bool ValidChannelName(const std::string &name);

    /*===== Server Console Input =====*/
    void handleConsoleInput();                              // Read from stdin
    void sendToAllClients(const std::string& text);         // Send message to all clients
    void welcomeServer();                                   // Print welcome message
    void welcomeClient(Client* client);                     // Send welcome message to client


    /*===== Utilities =====*/
    void sendError(int fd, const std::string& code,
        const std::string& target, const std::string& message);             // IRC error sender
    std::vector<std::string> splitCommand(const std::string& command);      // Split input string into tokens
    
    /*===== Global Signal Flag =====*/
    static int signal;


	void suppressChannel(const Channel & channel);
};

