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
void Server::handleCommand(Client* client, const Message& msg)
{
    if (msg.command == "PASS" || msg.command == "NICK" || msg.command == "USER" )
        handleRegistred(client, msg);
    else if (msg.command == "PING" || msg.command== "PRIVMSG" ||
			 msg.command== "MODE" || msg.command== "JOIN")
        handleServerCommand(client, msg);

	// handle channel 
		
    else if (!client->isRegistered())
        sendError(client->getFd(), "451", "*", "You have not registered");
    else
        sendError(client->getFd(), "421", msg.command, "Unknown command");
}


void Server::handleRegistred(Client* client, const Message& msg)
{
    if (msg.command == "PASS")
        handlePASS(client, msg);
    else if (msg.command == "NICK")
        handleNICK(client, msg);
    else if (msg.command == "USER")
        handleUSER(client, msg);
    else 
        return;
}

 void Server::handleServerCommand (Client* client, const Message& msg)
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
        return;
 }

void Server::sendError(int fd, const std::string& code, const std::string& target, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " :" + message + "\r\n";
    ssize_t sent = send(fd, full.c_str(), full.size(), 0);
    if (sent == -1)
        std::cerr << "Fail to send Error to Client [" << fd << "]\n";
}

void Server::sendError2(int fd, const std::string& code, const std::string& target, const std::string& targetBis, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " " + targetBis +" :" + message + "\n";
    send(fd, full.c_str(), full.size(), 0);
}

void Server::handlePASS(Client* client, const Message& msg)
{
    if (client->isRegistered())
    {
        sendError(client->getFd(), "462", "*", "You may not reregister");
        return;
    }
    if (msg.params.empty())
    {
        sendError(client->getFd(), "461", "PASS", "Not enough parameters");
        return;
    }

    if (msg.params[0] != _password)
    {
        sendError(client->getFd(), "464", "*", "Password incorrect");
        return;
    }
    client->setPass(msg.params[0]);
    client->setHasPass(true);
    if (client->hasNick() && client->hasUser() && client->hasPass())
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
    if (client->hasNick() && client->hasUser() && client->hasPass())
    {
        completeRegistration(client);
    }
}

void Server::handleUSER(Client* client, const Message& msg)
{
    if (client->isRegistered())
    {
        sendError(client->getFd(), "462", "*", "You may not register");
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
    if (client->hasNick() && client->hasUser() && client->hasPass())
    {
        completeRegistration(client);
    }

}

void Server::completeRegistration(Client* client)
{
    client->setRegistered(true);
	this->welcomeServer();
	this->welcomeClient(client);
}


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

void Server::welcomeClient(Client* client) 
{
	std::string nick = client->getNickname();

	client->send_msg(":" + _server_name + " 001 " + nick +
	                 " :\x03""03""Welcome to the GOSSIP.IRC Network, " + nick + "\x0F");

	client->send_msg(":" + _server_name + " 002 " + nick +
	                 " :\x03""03""Your host is " + _server_name + ", running version 1.0\x0F");

	client->send_msg(":" + _server_name + " 003 " + nick +
	                 " :\x03""03""This server was created July 2025\x0F");

	client->send_msg(":" + _server_name + " 004 " + nick +
	                 " :\x03""03" + _server_name + " 1.0 o o\x0F");

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


void Server::handlePING(Client* client, const Message& msg)
{
    if (msg.params.empty())
    {
        sendError(client->getFd(), "409", "*", "No origin specified");
        return;
    }
    std::string token = msg.params[0];

    std::string response = ":" + _server_name + " PONG :" + token;
    client->send_msg(response);
    client->send_msg( _server_name + " :PONG sent by the server");

}


// void Server::handleMODE (Client* client, const Message& msg)
// {
// 	//ici juste repondre au mode +i 
// 	// if channel existe , parsing de mode + appel fonction Sandrine
//     (void)client;
//     (void)msg;
// }

void Server::handleMODE(Client *client, const Message &msg)
{
	(void)client;
	//int i = 0;
	const std::string &target = msg.params[0];  // #channel
    const std::string &modes  = msg.params[1];  // +it, -o, +k, etc.

	//Ajouter une verif si le channel existe

	if (msg.params.size() < 2) 
	{
        sendError(client->getFd(), "461", "MODE", "Not enough parameters");
        return;
    }
	

	std::cout << "[DEBUG]target = " << target << "\n";
    std::cout << "[DEBUG]modes  = " << modes  << "\n";
	
	if (msg.params[0][0] == '+')
	{
		std::cout << " C'est un +" << std::endl;
		//
	}
	else if (msg.params[0][0] == '-') 
	{
		std::cout << "[DEBUG] C'est un -" << std::endl;
		//
	}
}

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
    if (p0 == "0"|| p0 == "#0")
    {
        channel = "0";
        return true;
    }
	if (p0.empty() || p0[0] != '#') // nom du channel doit commencer par # donc petite verif
		return (false);
	channel = p0;
	if (msg.params.size() >= 2) // si il y a un deuxieme paramettre c'est le mdp
		key = msg.params[1];
	return (true);
}
std::string toLower(const std::string &str) {
    std::string lower;
    for (size_t i = 0; i < str.size(); ++i)
        lower += std::tolower(str[i]);
    return lower;
}

void Server::handleJOIN(Client *client, const Message &msg)
{
	std::string channel, key, user;
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "JOIN", "Not enough parameters");
		return ;
	}
    if (!client->isRegistered())
	{
		sendError(client->getFd(), "451", client->getNickname(), "You have not registered");
        return;
    }
    if (!msg.prefix.empty() && toLower(msg.prefix) != toLower(client->getNickname()))    
    {
        sendError(client->getFd(), "451", client->getNickname(), "prefix different from user nickname");
        return;
    }

    if (msg.params[0] == "0" || msg.params[0] == "#0")
	{
        leaveAllChannels(client);
		return;
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
		if (!parseJoin(msg_chan,tmpChannel, tmpKey)) 
		{
			sendError(client->getFd(), "403", chan, "No such channel");
			continue;
		}
        if (ClientChannelCount(client) >= 10)
        {
			std::cout << "Client ["<< client->getFd() <<"] has 10 CHannels\n";
            sendError(client->getFd(), "405", channel, "You have joined too many channels");
            return;
        }
		handleSingleJoin(client, chan, key); // logique unique join
	}
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


std::vector<std::string> Server::splitComma(const std::string &input)
{
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string item;

	while (std::getline(ss, item, ','))
		result.push_back(item);
	return result;
}

void Server::handleSingleJoin(Client *client, const std::string &channelName, const std::string &key)
{
    std::cout << "[DEBUG][JOIN] Handle JOIN request\n";
	std::cout << "[DEBUG]  user    = " << client->getNickname() << "\n";
	std::cout << "[DEBUG]  channel = " << channelName << "\n";
	std::cout << "[DEBUG]  key     = " << (key.empty() ? "(no key)" : key) << "\n";
	// Vérifie que le nom est valide
	if (channelName.empty() || channelName[0] != '#')
	{
		sendError(client->getFd(), "403", channelName, "No such channel");
		return;
	}

	// Cherche le channel
	Channel* chan = findChannel(channelName);

	// Si existe pas, crer
	if (!chan)
	{
		std::cout << "[JOIN] Channel '" << channelName << "' does not exist. Creating it.\n";
		Channel* newChannel = new Channel(channelName);
		_channels.push_back(newChannel);
		chan = newChannel;
	}
	else
	{
		std::cout << "[JOIN] Channel '" << channelName << "' already exists.\n";
	}
	// Si client dans le channel
	if (chan->verifClientisUser(client))
    {
        std::cout << "[JOIN] Client already in channel '" << channelName << "'. Skipping.\n";
        return;
    }
	// Ajouter le client au channel
	chan->addUser(this, client, key);

	// ajouter reponse facon IRC a faire demain

	std::cout << "[DEBUG][JOIN] Client " << client->getNickname()
			  << "[DEBUG] joined channel " << channelName
			  << (key.empty() ? "[DEBUG] (no key)" : "[DEBUG] with key") << ".\n";
    std::cout << GREEN << BOLD << "Client successfully added to channel" << RESET << std::endl;

    std::cout << "[DEBUG] Clients in channel after join:\n";
    chan->printClientVectors();

}

void Server::leaveAllChannels(Client *client) 
{
    std::cout << BOLD << "Client [" << client->getFd() << "] leaving all channels\n";
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); )
    {
        Channel* chan = *it;

        std::cout << "[DEBUG]  -> Checking if client is in channel: " << chan->getName() << "\n";
        std::cout << "[DEBUG]     client ptr = " << client << "\n";

        std::cout << "[DEBUG]    users in channel:\n";
        chan->printUsers();

        if (chan->verifClientisUser(client) || chan->verifClientisOperator(client))
        {
            std::cout << "[DEBUG]  -> client is present in channel!\n";
            chan->removeUser(client->getFd());
            chan->removeOperator(client);

            std::string partMsg = ":" + client->getPrefix() + " PART " + chan->getName() + " :Leaving all channels\r\n";
            chan->ChannelSend(partMsg, client);

            std::cout << "[DEBUG]  -> left channel " << chan->getName() << "\n";
        }
        int result = chan->getClientCount();  // ou isChannelEmpty()

        std::cout << "[DEBUG] Clients in channel before delete:\n";
        chan->printClientVectors();
        std::cout << "[DEBUG] Get Client Count Result = " << result << std::endl;
        // Supprimer le channel si vide
        if (result == 0)
        {
            std::cout << "[DEBUG]  -> channel " << chan->getName() << " is now empty. Deleting it.\n";
            delete chan;
            it = _channels.erase(it); // erase retourne l'itérateur suivant
        }
        else
        {
            ++it;
        }
    }
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

