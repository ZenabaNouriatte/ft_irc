#pragma once
#include "Client.hpp"
#include "Message.hpp"
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <poll.h>
#include <sstream>
#include <string.h>
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
	int _port;
	std::string _password;
	int _server_fd;
	std::vector<pollfd> _poll_fds;
	std::string _server_name;
	std::map<int, Client *> _clients;

	void handleConsoleInput();

  public:
	Server(int port, const std::string &password);
	~Server();
	void start();
	void handleError(const std::string &message);
	void acceptNewClient();
	void handleClient(int client_fd);
	void removeClient(int client_fd);
	void cleanExit();

	static int signal;
	static void catchSignal(int);

	void completeRegistration(Client *client);
	void handleRegistred(Client *client, const Message &msg);
	void broadcast(const std::string &text);
	void welcomeServer();
	void welcomeClient(Client *client);

	void handleCommand(Client *client, const Message &msg);
	void handlePASS(Client *client, const Message &msg);
	void handleNICK(Client *client, const Message &msg);
	void handleUSER(Client *client, const Message &msg);
	void handleServerCommand(Client *client, const Message &msg);
	void handlePING(Client *client, const Message &msg);
	void handleMODE(Client *client, const Message &msg);
	void handleJOIN(Client *client, const Message &msg);
	bool parseJoin(const Message &msg, std::string &channel, std::string &key);
	bool PrefixUser(const Message &msg, std::string &User,
		std::string &channel, std::string &key);
    std::string userPrefix(const std::string& prefix);

	void sendError(int fd, const std::string &code, const std::string &target, const std::string &message);
};

