#pragma once
#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <cstring>
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
class	Channel;

class Server
{
  private:

    int                     _port;
    std::string             _password;
    int                     _server_fd;
    std::vector<pollfd>     _poll_fds;
    std::string             _server_name;
    std::map<int, Client*>  _clients;
    std::vector<Channel*>    _channels;

	public:
	/*===== Constructors / Destructor =====*/
	Server(int port, const std::string &password);
	~Server();

	/*===== Startup / Signal Handling / Error =====*/
	void                            start();
	static void                     catchSignal(int);
	void                            handleError(const std::string &message);


    /*===== GETTERS =====*/

    const std::string               &getServerName() const;

	/*===== Server Setup & Client Connection Management =====*/
	bool                            setupServerSocket(); 
	void                            setupPollFds();      
	void                            printStart();     
	void                            handlePollEvents();  
	void                            acceptNewClient();
	

	/*===== Client Communication =====*/
    void                            handleClient(int client_fd);                                                           
    void                            handleClientRead(Client* client, const std::string& input);                            
    void                            handleClientDisconnection(Client* client, int client_fd, ssize_t received_bytes);      


	/*===== MODE =====*/

	bool                            isValidModeCommand(Client *client, const Message &msg);
	void                            parseModeBlock(Client *client, Channel *chan, const Message &msg,
		                                const std::string &modes, size_t &paramIndex);
	void                            handleModeWithParam(Client *client, Channel *chan, const Message &msg,
		                                char c, char sign, size_t &paramIndex);
	void                            handleModeNoParam(Client *client, Channel *chan, char c, char sign);

    /*===== IRC Command Handling =====*/
    void                            handleCommand(Client* client, const Message& msg);
    void                            completeRegistration(Client* client);
    void                            handleRegistred(Client* client, const Message& msg);
    void                            handleServerCommand (Client* client, const Message& msg);
    
    void                            handlePASS (Client* client, const Message& msg);
    void                            handleNICK (Client* client, const Message& msg);
    void                            handleUSER (Client* client, const Message& msg);
    void                            handleQUIT(Client* client, const Message& msg);
    void                            handlePRIVMSG(Client* client, const Message& msg);
    void                            handleWHOIS(Client* client, const Message &msg);
	void                            handleKICK(Client *client, const Message &msg);
	void                            handleINVIT(Client *client, const Message &msg);
	void                            handleTOPIC(Client *client, const Message &msg);
    void                            handlePART(Client* client, const Message& msg);
    void                            handlePING (Client* client, const Message& msg);
    void                            handleMODE (Client* client, const Message& msg);
    void                            handleJOIN (Client* client, const Message& msg);

    /*===== IRC Command Helpers =====*/
    bool                            isNicknameInUse(const std::string &nick);
    bool                            MsgToChannel(Client* sender, const std::string& channelName, const std::string& message);
    bool                            PvMsgToUser(Client* sender, const std::string& target, const std::string& message);
    bool                            parseJoin(const Message &msg, std::string &channel, std::string &key);
	bool                            PrefixUser(const Message &msg, std::string &User,
	                                    std::string &channel, std::string &key);
    std::string                     userPrefix(const std::string& prefix);
    Channel*                        findChannel(const std::string& name);
	Client *                        findClient(const std::string &nickname);
    std::vector<std::string>        splitComma(const std::string &input);
    void                            handleSingleJoin(Client *client, const std::string &channelName, const std::string &key);
    bool                            ValidChannelName(const std::string &name);
    void                            leaveAllChannels(Client *client);
    int                             ClientChannelCount(Client* client) const;
	void                            verifIfCloseChannel(Channel* channel);
	void                            commandPart(Client* client, Channel* channel, std::string comment);
    void                            sendNameList(Client* client, Channel* chan);
    void                            sendTopic(Client* client, Channel* chan);
    void                            sendJoinMsg(Client* client, Channel* chan);


    /*===== Server Console Input =====*/
    void                            handleConsoleInput();                             
    void                            sendToAllClients(const std::string& text);         
    void                            welcomeServer();                                   
    void                            welcomeClient(Client* client);                     


    /*===== Utilities =====*/
    void                            sendError(int fd, const std::string& code,
                                        const std::string& target, const std::string& message);             
	void                            sendError2(int fd, const std::string& code, 
		                                const std::string& target, const std::string& targetBis, const std::string& message);
	std::vector<std::string>        splitCommand(const std::string& command);     
    Client*                         findByNick(const std::string& nickname);

    /*===== Global Signal Flag =====*/
    static int                      signal;

    /*===== QUIT =====*/
    void                            cleanExit();
    void                            cleanupChannels();
	void                            suppressChannel(const Channel & channel);
    void                            disconnectClient(int clientFd);
};
