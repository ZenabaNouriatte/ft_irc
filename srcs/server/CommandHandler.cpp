/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:34 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/24 15:35:20 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*========== METHODES ==========*/

void Server::welcomeServer()
{
	std::stringstream ss;
	ss << _port;
	std::cout << "\n██████████████████████████████████████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "███  ***  GOSSIP.IRC SERVER  ***  ████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "██████████████████████████████████████" << std::endl;
	std::cout << "\n────────────────────────────\n";
	std::cout << "Password : " << _password << std::endl;
	std::cout << "Port     : " << ss.str() << std::endl;
	std::cout << "────────────────────────────\n" << std::endl;
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

/*============================================================================*/

void Server::handleCommand(Client *client, const Message &msg)
{
	// std::cout << BLUE << BOLD << "RAW MSG [" << msg.raw << "]" << RESET << std::endl;
	// std::cout << BLUE << "COMMAND [" << msg.command << "]" << RESET << std::endl;
	if (msg.command == "PASS" || msg.command == "NICK" || msg.command == "USER")
		handleRegistred(client, msg);
	else if (msg.command == "PING" || msg.command == "MODE"
		|| msg.command == "JOIN")
		handleServerCommand(client, msg);
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

void Server::completeRegistration(Client *client)
{
	client->setRegistered(true);
	this->welcomeServer();
	this->welcomeClient(client);
}

void Server::handlePASS(Client *client, const Message &msg)
{
	std::cout << GREEN << BOLD << "COMMAND PASS received with: " << RESET << msg.params[0] << std::endl;
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "PASS", "Not enough parameters");
		return ;
	}
	if (client->isRegistered())
	{
		sendError(client->getFd(), "462", "*", "You may not register");
		return ;
	}
	if (msg.params[0] != _password)
	{
		sendError(client->getFd(), "464", "*", "Password incorrect");
		removeClient(client->getFd());
		return ;
	}
	client->setPass(msg.params[0]);
	client->setHasPass(true);
	if (client->hasNick() && client->hasUser() && client->hasPass())
		completeRegistration(client);
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
		completeRegistration(client);
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
		completeRegistration(client);
}

/*====================================================================*/

void Server::handleServerCommand(Client *client, const Message &msg)
{
	if (msg.command == "PING")
		handlePING(client, msg);
	else if (msg.command == "MODE")
		handleMODE(client, msg);
	else if (msg.command == "JOIN")
		handleJOIN(client, msg);
	else
		return ;
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

void Server::handleMODE(Client *client, const Message &msg)
{
	(void)client;
	(void)msg;
}

/*=============================================*/

std::string Server::userPrefix(const std::string& prefix)
{
    if (prefix.empty())
        return "";
		
    size_t find = prefix.find('!'); // Dans IRC, le '!' sépare le nick du reste (user@host).
    if (find!= std::string::npos)  // Si on a trouvé un '!', on retourne uniquement la partie avant : le nick.
        return prefix.substr(0, find);   // Aucun '!' trouvé : le préfixe est soit déjà un simple nick, soit un nom de serveur.
    return prefix;
}

bool	Server::PrefixUser(const Message &msg, std::string &User,
		std::string &channel, std::string &key)
{
	User.clear();
	channel.clear();
	key.clear();
	if (msg.prefix.empty())
		return (false);
		
	User = userPrefix(msg.prefix);

	if (!Server::parseJoin(msg, channel, key))
		return (false);
	return (true);
}

bool Server::parseJoin(const Message &msg, std::string &channel,
	std::string &key)
{
	channel.clear(); // reset
	key.clear();
	const std::string &p0 = msg.params[0]; // nom du channel
	if (msg.params.empty())
		return (false);
	if (p0.empty() || p0[0] != '#') // nom du channel doit commencer par # donc petite verif
		return (false);
	channel = p0;
	if (msg.params.size() >= 2) // si il y a un deuxieme paramettre c'est le mdp
		key = msg.params[1];
	return (true);
}

void Server::handleJOIN(Client *client, const Message &msg)
{
	std::string channel, key, user;
	if (!client->isRegistered())
	{
		sendError(client->getFd(), "451", "*", "You have not registered");
		return ;
	}
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "JOIN", "Not enough parameters");
		return ;
	}
	
	// chercher si le channel existe ici

	if (PrefixUser(msg,user,channel, key))
	{
		std::cout << "[PARSE] JOIN with user\n";
		std::cout << " user = " << user << "\n";
		std::cout << "  channel = " << channel << "\n";
		std::cout << "  key     = " << (key.empty() ? "(no key)" : key) << "\n";
		return ;
	}
	if (parseJoin(msg, channel, key))
	{
		std::cout << "[PARSE] JOIN \n";
		std::cout << "  channel = " << channel << "\n";
		std::cout << "  key     = " << (key.empty() ? "(no key)" : key) << "\n";
		return ;
	}
}
