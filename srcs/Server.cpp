/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:56 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 15:44:25 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int Server::signal = 0;

/*==========================================
      CONSTRUCTION DESTRUCTION SERVEUR        
==========================================*/

Server::Server(int port, const std::string& password) : _port(port), _password(password), _server_fd(-1)
{
    _server_name = "gossip.irc.localhost";
    
}

Server::~Server() 
{
    cleanExit();
    std::cout << RED << BOLD << "SERVER EXIT" << RESET << std::endl;
}

const std::string &Server::getServerName() const 
{
    return _server_name;
}

/*==========================================
                  METHODES        
==========================================*/

/*==========CONNEXION SETUP ==========*/

// Add server socket and stdin to poll()
void Server::setupPollFds()
{
    pollfd pfd;
    pfd.fd = _server_fd;
    pfd.events = POLLIN;
    _poll_fds.push_back(pfd);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    pollfd p_stdin;
    p_stdin.fd = STDIN_FILENO;
    p_stdin.events = POLLIN;
    _poll_fds.push_back(p_stdin);
}

void Server::printStart()
{
    std::cout << "\n────────────────────────────────\n";
    std::cout << BOLD << "Password             : " << _password << std::endl;
    std::cout << BOLD << "Server ready on port : " << _port << std::endl;
    std::cout << "────────────────────────────────\n" << std::endl;
}

// Create, bind, listen on socket
bool Server::setupServerSocket()
{
    int opt = 1;
    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1)
        return handleError("Fail socket"), false;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        return handleError("Fail setsockopt"), false;
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_server_fd, (sockaddr*)&addr, addr_len) == -1)
        return handleError("Fail bind"), false;
    if (listen(_server_fd, 5) == -1)
        return handleError("Fail listen"), false;
    return true;
}

/*========== MAIN LOOP ==========*/

void Server::start()
{
    if (!setupServerSocket())
        return;
    setupPollFds();
    printStart();
    while (Server::signal == 0)
    {
        int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 1000);
        if (poll_count < 0)
        {
            if (errno == EINTR)
                continue;
            return handleError("poll failed");
        }
        if (Server::signal != 0)
            break;
        handlePollEvents();
    }
}

// Process events returned by poll()
void Server::handlePollEvents()
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
    {
        int fd = _poll_fds[i].fd;

        if (_poll_fds[i].revents & (POLLHUP | POLLERR))
        {
            disconnectClient(fd);
            i--;
        }
        else if (_poll_fds[i].revents & POLLIN)
        {
            if (fd == _server_fd)
                acceptNewClient();
            else if (fd == STDIN_FILENO)
                handleConsoleInput();
            else
                handleClient(fd);
        }
    }
}

/*========== NEW CLIENT & EXISTING CLIENT ==========*/

void Server::acceptNewClient()
{
    sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(_server_fd, (sockaddr*)&client_addr, &addr_len);
    if (client_fd == -1)
        return handleError("Fail accept");
    std::cout << GREEN << BOLD << "Client [" << client_fd << "]" << " connected, wainting for register" << RESET << std::endl;
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _poll_fds.push_back(pfd);
    Client *client = new Client(client_fd);
    _clients[client_fd] = client;

}

// Read data from client
void Server::handleClient(int client_fd)
{
    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it == _clients.end()) 
    {
        std::cerr << "[ERROR] Client not found on fd [" << client_fd << "]" << std::endl;
        return;
    }
    Client* client = it->second;
    char buffer[1024];
    ssize_t received_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (received_bytes > 0)
    {
        buffer[received_bytes] = '\0';
        handleClientRead(client, buffer);
    }
    else
        handleClientDisconnection(client, received_bytes);
}

// Handle full commands
void Server::handleClientRead(Client* client, const std::string& input)
{
    int clientFd = client->getFd();
    client->appendToBuffer(input);
    std::vector<std::string> commands = client->extractCompleteCommands();

    for (size_t i = 0; i < commands.size(); ++i)
    {
        std::map<int, Client*>::iterator it = _clients.find(clientFd);
        if (it == _clients.end()) 
        {
            std::cout << "Client " << clientFd << " was disconnected during command processing" << std::endl;
            return; 
        }
        client = it->second;
        const std::string& raw_message = commands[i];
        std::cout << BOLD << "Client [" << clientFd << "] [RECV] " 
                  << raw_message << RESET << std::endl;
        Message msg(raw_message);
        handleCommand(client, msg);
    }
}

/*========== SERVER CONSOLE ==========*/

void Server::handleConsoleInput()
{
    char    buf[BUFFER_SIZE];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf) - 1);

    if (n <= 0)
        return;
    buf[n] = '\0';
    std::string input(buf);
    while (!input.empty() &&
           (input[input.size() - 1] == '\n' || input[input.size() - 1] == '\r'))
    {
        input.erase(input.size() - 1, 1);
    }
    if (!input.empty())
        sendToAllClients(input);
}

void Server::sendToAllClients(const std::string& text)
{
    const std::string prefix = ":gossip.irc.localhost NOTICE * :";
    std::string irc_line = prefix + text + "\r\n";
    for (std::map<int, Client*>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        it->second->send_msg(irc_line);
    }
    std::cout << GREEN << BOLD << "[Serveur] " << irc_line << RESET;
}

/*========== OTHER ==========*/

std::vector<std::string> Server::splitCommand(const std::string& command) 
{
    std::vector<std::string> tokens;
    std::istringstream iss(command);
    std::string token;

    while (iss >> token) 
    {
        tokens.push_back(token);
    }
    return tokens;
}

void Server::handleError(const std::string& message) 
{
    std::cerr << message << std::endl;
    if (_server_fd >= 0)
        close(_server_fd);
}

void Server::catchSignal(int signum)
{
    signal = signum;
}


