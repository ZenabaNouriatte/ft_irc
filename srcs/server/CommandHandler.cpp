/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CommandHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:34 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/25 19:17:08 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
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
		|| msg.command == "MODE" || msg.command == "JOIN")
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

void Server::handlePRIVMSG(Client *client, const Message &msg)
{
	if (!client->isRegistered())
	{
		sendError(client->getFd(), "451", "*", "You have not registered");
		return ;
	}
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "PRIVMSG", "Not enough parameters");
		return ;
	}
	std::string target = msg.params[0];
	std::string messageText = msg.trailing;
	if (messageText.empty())
	{
		sendError(client->getFd(), "412", "PRIVMSG", "No text to send");
		return ;
	}
	std::cout << "[PRIVMSG] " << client->getNickname() << "	-> " << msg.params[0] << ": " << msg.trailing << std::endl;
	// si le param 0 est le nom d'un client
	// envoyer au client
	// si le param 0 est une channel : envoyer au channel
	// TODO: envoye msg au bon client ou canal
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

std::string Server::userPrefix(const std::string &prefix)
{
	if (prefix.empty())
		return ("");
	size_t find = prefix.find('!');
		// Dans IRC,le '!' sépare le nick du reste (user@host).
	if (find != std::string::npos) 
		// Si on a trouvé un '!',on retourne uniquement la partie avant : le nick.
		return (prefix.substr(0, find));
	// Aucun '!' trouvé : le préfixe est soit déjà un simple nick soit un nom de serveur.
	return (prefix);
}

bool Server::PrefixUser(const Message &msg, std::string &User,
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
	if (p0.empty() || p0[0] != '#')
		// nom du channel doit commencer par # donc petite verif
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
	if (msg.params[0] == "0")
	{
		// a faire , quitter tous les channels ou le client est enregistre
		// si client
		std::cout << "[JOIN] User " << client->getNickname() << " leaving all channels\n";
		return ;
	}
	if (!msg.prefix.empty() && PrefixUser(msg, user, channel, key))
	{
		handleSingleJoin(client, channel, key);
		return ;
	}
	std::vector<std::string> channels = splitComma(msg.params[0]);
	std::vector<std::string> keys;
	if (msg.params.size() > 1)
		keys = splitComma(msg.params[1]);
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string chan = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";
		std::string tmpChannel, tmpKey;
		Message msg_chan("JOIN " + chan + " :" + key);
		if (!parseJoin(msg_chan, tmpChannel, tmpKey))
		{
			sendError(client->getFd(), "403", chan, "No such channel");
			return ;
		}
		handleSingleJoin(client, chan, key); // logique unique join
	}
}

std::vector<std::string> Server::splitComma(const std::string &input)
{
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string item;
	while (std::getline(ss, item, ','))
		result.push_back(item);
	return (result);
}

void Server::handleSingleJoin(Client *client, const std::string &channelName,
	const std::string &key)
{
	Channel	*chan;

	std::cout << "[JOIN] Handle JOIN request\n";
	std::cout << "  user    = " << client->getNickname() << "\n";
	std::cout << "  channel = " << channelName << "\n";
	std::cout << "  key     = " << (key.empty() ? "(no key)" : key) << "\n";
	// Vérifie que le nom est valide
	if (channelName.empty() || channelName[0] != '#')
	{
		sendError(client->getFd(), "403", channelName, "No such channel");
		return ;
	}
	// Cherche le channel
	chan = findChannel(channelName);
	// S'il n'existe pas, on le crée
	if (!chan)
	{
		std::cout << "[JOIN] Channel '" << channelName << "' does not exist. Creating it.\n";
		Channel newChannel(channelName);
		_channels.push_back(newChannel);
		chan = &_channels.back();
	}
	else
	{
		std::cout << "[JOIN] Channel '" << channelName << "' already exists.\n";
	}
	// Vérifie si le client est déjà dans le channel
	// if (chan->hasUser(*client))
	// {
	// 	std::cout << "[JOIN] Client already in channel '" << channelName << "'. Skipping.\n";
	// 	return ;
	// }
	// Ajouter le client au channel
	chan->addUser(*client);
	// ajouter reponse facon IRC a faire demain
	std::cout << "[JOIN] Client " << client->getNickname() << " joined channel " << channelName << (key.empty() ? " (no key)" : " with key") << ".\n";
	std::cout << GREEN << BOLD << "Client successfully added to channel" << RESET << std::endl;
}

Channel *Server::findChannel(const std::string &name)
{
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		if (_channels[i].getName() == name)
			return (&_channels[i]);
	}
	return (NULL);
}

Client* Server::findClient(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
        Client* client = it->second; // valeur dans la map
        if (client && client->getNickname() == nickname)
            return client;
    }
    return NULL;
}


/*======================================================*/

void Server::handleMODE(Client *client, const Message &msg)
{
	(void)client;
	Channel	*chan;
	// int i = 0;
	const std::string &channel = msg.params[0]; // #channel
	const std::string &modes = msg.params[1];  // +it, -o, +k, etc.
	const std::string &key = msg.params[2]; // mdp ???
	const std::string &target = msg.params[3]; // target ????
	int &limit = msg.params[4]; // limit ????

	//en realiter ey,target, limit sont en 3e arg,faut faire une nouvelle fonction
	//pour pouvoir classer en key,target ou limit

	// Ajouter une verif si le channel existe
	if (msg.params.size() < 2)
	{
		sendError(client->getFd(), "461", "MODE", "Not enough parameters");
		return ;
	}
	chan = findChannel(channel);
	if(!chan)
	{
		sendError(client->getFd(), "461", "MODE", "Channel existe pas ");
		return ;
	}

	std::cout << "channel = " << channel << "\n";
	std::cout << "modes  = " << modes << "\n";
	for (size_t i = 0; i < modes.size(); ++i)
	{
    	char c = modes[i];
   		switch (c)
		{
       		case 'i':
				std::cout << "C'est un i" << std::endl;
				chan->changeModeI(*client, modes);
				break ;
        	case 't':
				std::cout << "C'est un t" << std::endl;
				chan->changeModeT(*client, modes);
				break ;
        	case 'k':
				std::cout << "C'est un k" << std::endl;
				chan->changeModeK(*client, modes, key);
				break ;
        	case 'o':
				std::cout << "C'est un o" << std::endl;
				chan->changeModeK(*client, modes, target);
				break ;
        	case 'l':
				std::cout << "C'est un l" << std::endl;
				chan->changeModeL(*client, modes, limit);
				break ;
    	}
	}
}
