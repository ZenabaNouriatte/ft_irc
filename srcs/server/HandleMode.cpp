/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleMode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/30 12:31:48 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"


Client *Channel::findClientByNick(const std::string &nick)
{
	for (std::vector<Client>::iterator it = _users.begin(); it != _users.end(); ++it)
	{            
        // char sign = current[0];
		if (it->getNick() == nick)
			return (&(*it)); // renvoie l'adresse de l'objet dans le vector
	}
	for (std::vector<Client>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (it->getNick() == nick)
			return (&(*it));
	}
	return (NULL);
}

void Server::handleMODE(Client *client, const Message &msg`)
{
    if (!isValidModeCommand(client, msg))
        return;

    Channel *chan = findChannel(msg.params[0]);
    size_t paramIndex = 2;

    for (size_t i = 1; i < msg.params.size(); ++i)
    {
        std::string current = msg.params[i];

        if (!current.empty() && (current[0] == '+' || current[0] == '-'))
            parseModeBlock(client, chan, msg, current, paramIndex);
    }
}

bool Server::isValidModeCommand(Client *client, const Message &msg)
{
    if (msg.params.size() < 2 || msg.params[0][0] != '#')
    {
        sendError(client->getFd(), "461", "MODE", "Not enough parameters");
        return false;
    }

    Channel *chan = findChannel(msg.params[0]);
    if (!chan)
    {
        sendError(client->getFd(), "403", "MODE", "No such channel");
        return false;
    }

    std::cout << "[DEBUG] : Channel = " << msg.params[0] << std::endl;
    return true;
}

void Server::parseModeBlock(Client *client, Channel *chan, const Message &msg,
                            const std::string &modes, size_t &paramIndex)
{
    std::cout << "[DEBUG] : modes = " << modes << std::endl;
    char sign = modes[0];

    for (size_t j = 1; j < modes.size(); ++j)
    {
        char c = modes[j];

        if (c == 'o' || c == 'k' || c == 'l')
            handleModeWithParam(client, chan, msg, c, sign, paramIndex);
        else
            handleModeNoParam(client, chan, c, sign);
    }
}

void Server::handleModeWithParam(Client *client, Channel *chan,
                                 const Message &msg, char c, char sign, size_t &paramIndex)
{
    (void)sign;
    if (paramIndex >= msg.params.size())
    {
        std::cout << "Pas assez de paramètres pour le mode " << c << std::endl;
        return;
    }

    std::string param = msg.params[paramIndex];
    std::cout << "[DEBUG] : mode = " << c << " param = " << param << std::endl;

    if (c == 'o')
    {
        Client *target = chan->findClientByNick(param);
        if (!target)
            sendError(client->getFd(), "401", param, "No such nick");
        else
            std::cout << "C'est un o avec " << param << std::endl;
            // chan->changeModeO(*client, std::string(1, sign) + "o", *target);
    }
    else if (c == 'k')
    {
        std::cout << "C'est un k avec " << param << std::endl;
        // chan->changeModeK(*client, std::string(1, sign) + "k", param);
    }
    else if (c == 'l')
    {
        int limit = atoi(param.c_str());
        std::cout << "C'est un l avec " << limit << std::endl;
        // chan->changeModeL(*client, std::string(1, sign) + "l", limit);
    }

    paramIndex++;
}

void Server::handleModeNoParam(Client *client, Channel *chan, char c, char sign)
{
    (void)sign;
    (void)chan;
    (void)client;
    if (c == 'i')
    {
        std::cout << "C'est un i" << std::endl;
        // chan->changeModeI(*client, std::string(1, sign) + "i");
    }
    else if (c == 't')
    {
        std::cout << "C'est un t" << std::endl;
        // chan->changeModeT(*client, std::string(1, sign) + "t");
    }
    else
    {
        std::cout << "Mode inconnu : " << c << std::endl;
    }
}
