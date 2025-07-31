/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:34 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 14:02:59 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


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


/*======================== COMMAND ============================*/

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
