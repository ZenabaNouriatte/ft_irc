/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleCommand.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:34 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/05 14:18:04 by smolines         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Channel.hpp"
#include "Utils.hpp"


void Server::handleCommand(Client *client, const Message &msg)
{
	if (msg.command == "PASS" || msg.command == "NICK" || msg.command == "USER")
		handleRegistred(client, msg);
	else if (msg.command == "PING" || msg.command == "PRIVMSG"
		|| msg.command == "MODE" || msg.command == "JOIN" 
		|| msg.command == "TOPIC" || msg.command == "PART"
		|| msg.command == "KICK" || msg.command == "WHOIS" || msg.command == "QUIT")
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
	std::cout << "[DEBUG] HANDLE SERVER COMMAND , msg command [" << msg.command << "]" << std::endl;
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
	else if (msg.command == "PART")
		handlePART(client, msg);
	else if (msg.command == "WHOIS")
        handleWHOIS(client, msg);
    else if (msg.command == "QUIT")
        handleQUIT(client, msg);
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


bool Server::isNicknameInUse(const std::string &nick)
{
    for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (it->second->getNickname() == nick)
            return true;
    }
    return false;
}

void Server::handleNICK(Client *client, const Message &msg) 
{
    if (msg.params.size() < 1) 
    {
        sendError(client->getFd(), "431", "*", ":No nickname given");
        disconnectClient(client->getFd());
        return;
    }

    std::string requestedNick = msg.params[0];

    // Vérifier la validité du pseudonyme
    if (!isValidNickname(requestedNick)) 
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        sendError(client->getFd(), "432", currentNick, requestedNick + " :Erroneous nickname");
        disconnectClient(client->getFd());
        return;
    }

    // Si c'est le même pseudonyme que celui du client, ignorer
    if (client->getNickname() == requestedNick)
        return;

    // Vérifier si le pseudonyme est déjà utilisé
    if (isNicknameInUse(requestedNick)) 
    {
        std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();
        
        // Envoyer l'erreur 433 avec le bon format et déconnecter
        sendError(client->getFd(), "433", currentNick, requestedNick + " :Nickname is already in use");
        disconnectClient(client->getFd());
        return;
    }

    // Le pseudonyme est disponible et valide
    if (client->isRegistered() && !client->getNickname().empty()) 
    {
        // Client déjà enregistré qui change de pseudonyme
        std::string oldNick = client->getNickname();
        std::string nickMsg = ":" + oldNick + "!~" + client->getUsername() + 
                             "@localhost NICK :" + requestedNick + "\r\n";
        sendToAllClients(nickMsg);
    }

    client->setNickname(requestedNick);
    client->setHasNick(true);

    if (client->hasNick() && client->hasUser() && client->hasPass()) {
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



Client* Server::findByNick(const std::string& nickname)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if (toLower(it->second->getNick()) == toLower(nickname))
            return it->second;
    }
    return NULL;
}

void Server::handleWHOIS(Client* client, const Message &msg)
{
    std::cout << "DANS WHO IS\n";

    if (msg.params.empty())
    {
        client->send_msg(":" + _server_name + " 431 " + client->getNick() + " :No nickname given");
        return;
    }

    std::string target = msg.params[0];
    Client* targetClient = findByNick(target);

    if (!targetClient)
    {
        client->send_msg(":" + _server_name + " 401 " + client->getNick() + " " + target + " :No such nick");
        return;
    }

    // WHOIS user line (code 311)
    client->send_msg(":" + _server_name + " 311 " + client->getNick() + " " +
                    targetClient->getNick() + " ~user localhost * :" + targetClient->getRealname());

    // WHOIS end line (code 318)
    client->send_msg(":" + _server_name + " 318 " + client->getNick() + " " +
                    targetClient->getNick() + " :End of WHOIS list");

}

void Server::handleQUIT(Client *client, const Message &msg)
{
	std::string reason = "Client Quit";
	if (!msg.params.empty())
		reason = msg.params[0];

	// Construire le message de QUIT
	std::string quitMsg = ":" + client->getPrefix() + " QUIT :" + reason + "\r\n";

	// Prévenir tous les channels du départ du client
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); )
	{
		Channel *channel = *it;
		if (channel->verifClientisInChannel(client))
		{
			channel->ChannelSend(quitMsg, client);
			channel->removeUser(client->getFd());
			channel->removeOperator(client); // nettoyage optionnel

			if (channel->isChannelEmpty()) {
				delete channel;
				it = _channels.erase(it); // erase retourne le nouvel itérateur
				continue;
			}
		}
		++it;
	}

	// Déconnecter proprement le client
	disconnectClient(client->getFd());
}


