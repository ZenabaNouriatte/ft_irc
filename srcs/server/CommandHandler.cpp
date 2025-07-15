#include "Server.hpp"

void Server::handleCommand(Client* client, const Message& msg)
{
    if (msg.command == "PASS")
        handlePASS(client, msg);
    else if (msg.command == "NICK")
        handleNICK(client, msg);
    else if (msg.command == "USER")
        handleUSER(client, msg);
    else if (!client->isRegistered())
        sendError(client->getFd(), "451", "*", "You have not registered");
    else if (msg.command == "PING")
        handlePING(client, msg);
    else
        sendError(client->getFd(), "421", msg.command, "Unknown command");
}

void Server::sendError(int fd, const std::string& code, const std::string& target, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " :" + message + "\n";
    send(fd, full.c_str(), full.size(), 0);
}

void Server::handlePASS (Client* client, const Message& msg) 
{
    (void)client;
    (void)msg;
}
void Server::handleNICK (Client* client, const Message& msg)
{
    (void)client;
    (void)msg;
}

void Server::handleUSER (Client* client, const Message& msg)
{
    (void)client;
    (void)msg;
}

void Server::handlePING (Client* client, const Message& msg)
{
    (void)client;
    (void)msg;
}
