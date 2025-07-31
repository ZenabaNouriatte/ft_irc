/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:34 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 11:35:08 by smolines         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*========== METHODES ==========*/

void Server::welcomeServer()
{
	std::cout << "\n██████████████████████████████████████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "███  ***  GOSSIP.IRC SERVER  ***  ████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "██████████████████████████████████████" << std::endl;
	std::cout << "\n────────────────────────────\n" << std::endl;
	std::cout << "         SERVER LOG        :\n" << std::endl;
}

void Server::welcomeClient(Client *client)
{
	std::string nick = client->getNickname();
	client->send_msg(":" + _server_name + " 001 " + nick +
						" :\x03"
						"03"
						"Welcome to the GOSSIP.IRC Network, " +
						nick + "\x0F");
	client->send_msg(":" + _server_name + " 002 " + nick +
						" :\x03"
						"03"
						"Your host is " +
						_server_name + ", running version 1.0\x0F");
	client->send_msg(":" + _server_name + " 003 " + nick +
						" :\x03"
						"03"
						"This server was created July 2025\x0F");
	client->send_msg(":" + _server_name + " 004 " + nick +
						" :\x03"
						"03" +
						_server_name + " 1.0 o o\x0F");
}

void Server::sendError(int fd, const std::string &code,
	const std::string &target, const std::string &message)
{
	std::string full = ":" + _server_name + " " + code + " " + target + " :"
		+ message + "\n";
	send(fd, full.c_str(), full.size(), 0);
}

void Server::sendError2(int fd, const std::string& code, const std::string& target, const std::string& targetBis, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " " + targetBis +" :" + message + "\n";
    send(fd, full.c_str(), full.size(), 0);
}

void Server::completeRegistration(Client *client)
{
	client->setRegistered(true);
	this->welcomeServer();
	this->welcomeClient(client);
}

void Server::handleCommand(Client *client, const Message &msg)
{
	if (msg.command == "PASS" || msg.command == "NICK" || msg.command == "USER")
		handleRegistred(client, msg);
	else if (msg.command == "PING" || msg.command == "PRIVMSG"
		|| msg.command == "MODE" || msg.command == "JOIN" || msg.command == "KICK")
		handleServerCommand(client, msg);
	// handle channel
	else if (!client->isRegistered())
		sendError(client->getFd(), "451", "*", "You have not registered");
	else
		sendError(client->getFd(), "421", msg.command, "Unknown command");
}

void Server::handleRegistred(Client *client, const Message &msg)
{
	if (msg.command == "PASS")
		handlePASS(client, msg);
	else if (msg.command == "NICK")
		handleNICK(client, msg);
	else if (msg.command == "USER")
		handleUSER(client, msg);
	else
		return ;
}

void Server::handleServerCommand(Client *client, const Message &msg)
{
	if (msg.command == "PRIVMSG")
		handlePRIVMSG(client, msg);
	if (msg.command == "PING")
		handlePING(client, msg);
	else if (msg.command == "MODE")
		handleMODE(client, msg);
	else if (msg.command == "JOIN")
		handleJOIN(client, msg);
	else if (msg.command == "KICK")
		handleKICK(client, msg);
	else if (msg.command == "INVIT")
		handleINVIT(client, msg);
	else if (msg.command == "TOPIC")
		handleTOPIC(client, msg);
	else
		return ;
}

void Server::handlePASS(Client *client, const Message &msg)
{
	if (client->isRegistered())
	{
		sendError(client->getFd(), "462", "*", "You may not reregister");
		return ;
	}
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "PASS", "Not enough parameters");
		return ;
	}
	if (msg.params[0] != _password)
	{
		sendError(client->getFd(), "464", "*", "Password incorrect");
		return ;
	}
	client->setPass(msg.params[0]);
	client->setHasPass(true);
	if (client->hasNick() && client->hasUser() && client->hasPass())
	{
		completeRegistration(client);
	}
}

void Server::handleNICK(Client *client, const Message &msg)
{
	std::string newNick = msg.params[0];
	if (msg.params.size() < 1)
	{
		sendError(client->getFd(), "431", "*", "No nickname given");
		return ;
	}
	for (std::map<int,
		Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second != client && it->second->getNickname() == newNick)
		{
			sendError(client->getFd(), "433", "*",
				"Nickname is already in use");
			return ;
		}
	}
	if (newNick.find_first_of(" ,*?!@") != std::string::npos)
	{
		sendError(client->getFd(), "432", "*", "Error nickname");
		return ;
	}
	client->setNickname(newNick);
	client->setHasNick(true);
	if (client->hasNick() && client->hasUser() && client->hasPass())
	{
		completeRegistration(client);
	}
}

void Server::handleUSER(Client *client, const Message &msg)
{
	if (client->isRegistered())
	{
		sendError(client->getFd(), "462", "*", "You may not register");
		return ;
	}
	if (msg.params.size() < 2)
	{
		sendError(client->getFd(), "461", "USER", "Not enough parameters");
		return ;
	}
	client->setUsername(msg.params[0]);
	client->setRealname(msg.params[1]);
	client->setHasUser(true);
	if (client->hasNick() && client->hasUser() && client->hasPass())
	{
		completeRegistration(client);
	}
}

/*======================================================*/

void Server::handlePRIVMSG(Client* client, const Message& msg)
{
    if (!client->isRegistered())
    {
        sendError(client->getFd(), "451", "*", "You have not registered");
        return;
    }

    if (msg.params.empty())
    {
        sendError(client->getFd(), "461", "PRIVMSG", "Not enough parameters");
        return;
    }
	std::string target = msg.params[0];
    std::string messageText = msg.trailing;
    if (messageText.empty())
    {
        sendError(client->getFd(), "412", "PRIVMSG", "No text to send");
        return;
    }
	std::cout << "[PRIVMSG] " << client->getNickname() << " -> " << msg.params[0] << ": " << msg.trailing << std::endl;
	if (PvMsgToUser(client, target, messageText))
        return;

    
    if (target[0] == '#' && MsgToChannel(client, target, messageText))
    {
        // Try sending to channel if it starts with #
		return;
	}
	sendError(client->getFd(), "401", target, "No such nick/channel");

}

bool Server::PvMsgToUser(Client* sender, const std::string& target, const std::string& message)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* dest = it->second;
        if (dest->getNickname() == target)
        {
            std::string prefix = ":" + sender->getPrefix(); // pas de prefix dans client
            std::string irc_line = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
            dest->send_msg(irc_line);
            return true;
        }
    }
    return false;
}

bool Server::MsgToChannel(Client* sender, const std::string& channelName, const std::string& message)
{
    Channel* channel = findChannel(channelName);
    if (!channel)
    {
        sendError(sender->getFd(), "403", channelName, "No such channel");
        return false;
    }

    std::string irc_line = ":" + sender->getPrefix() + " PRIVMSG " + channelName + " :" + message + "\r\n";

    channel->ChannelSend(irc_line, sender);

    return true;
}

void Server::handlePING(Client *client, const Message &msg)
{
	if (msg.params.empty())
	{
		sendError(client->getFd(), "409", "*", "No origin specified");
		return ;
	}
	std::string token = msg.params[0];
	std::string response = ":" + _server_name + " PONG :" + token;
	client->send_msg(response);
	client->send_msg(_server_name + " :PONG sent by the server");
}


int Server::ClientChannelCount(Client* client) const 
{
    int count = 0;
    for (std::vector<Channel*>::const_iterator it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel* chan = *it;
        if (chan->verifClientisUser(client) || chan->verifClientisOperator(client))
            count++;
    }
    return count;
}




Channel* Server::findChannel(const std::string& name)
{
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		if (_channels[i]->getName() == name)
			return _channels[i];
	}
	return NULL;
}

Client *Server::findClient(const std::string &nickname)
{
	for (std::map<int,
		Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client *client = it->second; // valeur dans la map
		if (client && client->getNickname() == nickname)
			return (client);
	}
	return (NULL);
}