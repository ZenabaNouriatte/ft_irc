#include "Server.hpp"

/*========== METHODES ==========*/
void Server::handleCommand(Client* client, const Message& msg)
{
    std::cout << "COMMAND " << msg.command << std::endl;
    if (msg.command == "PASS")
        handlePASS(client, msg);
    else if (msg.command == "NICK")
        handleNICK(client, msg);
    else if (msg.command == "USER")
        handleUSER(client, msg);
    else if (!client->isRegistered())
        sendError(client->getFd(), "451", "*", "You have not registered");
    else
        sendError(client->getFd(), "421", msg.command, "Unknown command");
}

void Server::sendError(int fd, const std::string& code, const std::string& target, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " :" + message + "\n";
    send(fd, full.c_str(), full.size(), 0);
}

void Server::handlePASS(Client* client, const Message& msg)
{
    if (msg.params.empty())
    {
        sendError(client->getFd(), "461", "PASS", "Not enough parameters");
        return;
    }
    if (client->isRegistered())
    {
        sendError(client->getFd(), "462", "*", "You may not reregister");
        return;
    }
    if (msg.params[0] != _password)
    {
        sendError(client->getFd(), "464", "*", "Password incorrect");
        removeClient(client->getFd());
        return;
    }
    client->setHasPass(true);
    if (client->isRegistered())
    {
        completeRegistration(client);
    }

}


void Server::handleNICK(Client* client, const Message& msg)
{
    std::string newNick = msg.params[0]; 
    if (msg.params.size() < 1)
    {
        sendError(client->getFd(), "431", "*", "No nickname given");
        return;
    }

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second != client && it->second->getNickname() == newNick)
        {
            sendError(client->getFd(), "433", "*", "Nickname is already in use");
            return;
        }
    }
    if (newNick.find_first_of(" ,*?!@") != std::string::npos)
    {
        sendError(client->getFd(), "432", "*", "Error nickname");
        return;
    }
    client->setNickname(newNick);
    client->setHasNick(true);
    if (client->isRegistered())
    {
        completeRegistration(client);
    }

}

void Server::handleUSER(Client* client, const Message& msg)
{
    if (client->isRegistered())
    {
        sendError(client->getFd(), "462", "*", "You may not reregister");
        return;
    }
    if (msg.params.size() < 2)
    {
        sendError(client->getFd(), "461", "USER", "Not enough parameters");
        return;
    }
    client->setUsername(msg.params[0]);
    client->setRealname(msg.params[1]);
    client->setHasUser(true);
    if (client->isRegistered())
    {
        completeRegistration(client);
    }
}

void Server::completeRegistration(Client* client)
{

    client->setRegistered(true);

    std::string nick = client->getNickname();

    client->send_msg(":" + _server_name + " 001 " + nick +
                     " :Welcome to the IRC Network, " + nick);

    client->send_msg(":" + _server_name + " 002 " + nick +
                     " :Your host is " + _server_name + ", running version 1.0");

    client->send_msg(":" + _server_name + " 003 " + nick +
                     " :This server was created July 2025");

    client->send_msg(":" + _server_name + " 004 " + nick +
                     " " + _server_name );
}
