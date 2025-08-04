/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleMode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/04 12:51:57 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

Client* Channel::findClientByNick(const std::string &nick)
{
	for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	return (NULL);
}


void Server::handleMODE(Client *client, const Message &msg)
{
	Channel	*chan;
	size_t	paramIndex;

	if (!isValidModeCommand(client, msg))
		return ;
	chan = findChannel(msg.params[0]);
	paramIndex = 2;
	for (size_t i = 1; i < msg.params.size(); ++i)
	{
		std::string current = msg.params[i];
		if (!current.empty() && (current[0] == '+' || current[0] == '-'))
			parseModeBlock(client, chan, msg, current, paramIndex);
	}
}

bool Server::isValidModeCommand(Client *client, const Message &msg)
{
	Channel	*chan;

	if (msg.params.size() < 2 || msg.params[0][0] != '#')
	{
		sendError(client->getFd(), "461", "MODE", "Not enough parameters");
		return (false);
	}
	chan = findChannel(msg.params[0]);
	if (!chan)
	{
		sendError(client->getFd(), "403", "MODE", "No such channel");
		return (false);
	}
	std::cout << "[DEBUG] : Channel = " << msg.params[0] << std::endl;
	return (true);
}

void Server::parseModeBlock(Client *client, Channel *chan, const Message &msg,
	const std::string &modes, size_t &paramIndex)
{
	char	sign;
	char	c;

	std::cout << "[DEBUG] : modes = " << modes << std::endl;
	sign = modes[0];
	for (size_t j = 1; j < modes.size(); ++j)
	{
		c = modes[j];
		if (c == 'o' || c == 'k' || c == 'l')
			handleModeWithParam(client, chan, msg, c, sign, paramIndex);
		else
			handleModeNoParam(client, chan, c, sign);
	}
}

void Server::handleModeWithParam(Client *client, Channel *chan,
	const Message &msg, char c, char sign, size_t &paramIndex)
{
	Client	*target;
	int		limit;

	(void)sign;
	if (paramIndex >= msg.params.size())
	{
		std::cout << "Pas assez de paramètres pour le mode " << c << std::endl;
		return ;
	}
	std::string param = msg.params[paramIndex];
	std::cout << "[DEBUG] : mode = " << c << " param = " << param << std::endl;
	if (c == 'o')
	{
		target = chan->findClientByNick(param);
		if (!target)
			sendError(client->getFd(), "401", param, "No such nick");
		else
		{
			std::cout << "C'est un o avec " << param << std::endl;
			chan->changeModeO(this,client, std::string(1, sign) + "o", target);
		}
	}
	else if (c == 'k')
	{
		std::cout << "C'est un k avec " << param << std::endl;
		chan->changeModeK(this,client, std::string(1, sign) + "k", param);
	}
	else if (c == 'l')
	{
		limit = atoi(param.c_str());
		std::cout << "C'est un l avec " << limit << std::endl;
		chan->changeModeL(this, client, std::string(1, sign) + "l", limit);
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
		chan->changeModeI(this,client, std::string(1, sign) + "i");
	}
	else if (c == 't')
	{
		std::cout << "C'est un t" << std::endl;
		chan->changeModeT(this,client, std::string(1, sign) + "t");
	}
	else
	{
		std::cout << "Mode inconnu : " << c << std::endl;
	}
}
