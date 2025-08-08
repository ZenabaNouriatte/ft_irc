/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleMode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 14:01:40 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

Client *Channel::findClientByNick(const std::string &nick)
{
	for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getNick() == nick)
			return (*it);
	}
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
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
	bool	hasParam;

	sign = modes[0];
	for (size_t j = 1; j < modes.size(); ++j)
	{
		c = modes[j];
		hasParam = paramIndex < msg.params.size();
		if (c == 'o')
			handleModeWithParam(client, chan, msg, c, sign, paramIndex);
		else if (c == 'k')
		{
			if (hasParam)
				handleModeWithParam(client, chan, msg, c, sign, paramIndex);
			else
				handleModeNoParam(client, chan, c, sign);
		}
		else if (c == 'l')
		{
			if (hasParam)
				handleModeWithParam(client, chan, msg, c, sign, paramIndex);
			else
				handleModeNoParam(client, chan, c, sign);
		}
		else
			handleModeNoParam(client, chan, c, sign);
	}
}

void Server::handleModeWithParam(Client *client, Channel *chan,
	const Message &msg, char c, char sign, size_t &paramIndex)
{
	Client	*target;
	int		limit;

	if (paramIndex >= msg.params.size())
	{
		std::cout << "[DEBUG]  Pas assez de paramètres pour le mode " << c << std::endl;
		return ;
	}
	std::string param = msg.params[paramIndex];
	if (c == 'o')
	{
		target = chan->findClientByNick(param);
		if (!target)
		{
			sendError(client->getFd(), "401", param, "No such nick/channel");
			sendError2(client->getFd(), "441", param, chan->getName(),
				"They aren't on that channel");
		}
		else
		{
			chan->changeModeO(this, client, std::string(1, sign) + "o", target);
		}
	}
	else if (c == 'k')
	{
		chan->changeModeK(this, client, std::string(1, sign) + "k", param);
	}
	else if (c == 'l')
	{
		limit = atoi(param.c_str());
		chan->changeModeL(this, client, std::string(1, sign) + "l", limit);
	}
	paramIndex++;
}

void Server::handleModeNoParam(Client *client, Channel *chan, char c, char sign)
{
	if (c == 'i')
		chan->changeModeI(this, client, std::string(1, sign) + "i");
	else if (c == 't')
		chan->changeModeT(this, client, std::string(1, sign) + "t");
	else if (c == 'k')
	{
		std::cout << "DEBUG C'est un k (sans param)" << std::endl;
		chan->changeModeK(this, client, std::string(1, sign) + "k", "");
	}
	else if (c == 'l')
	{
		std::cout << "DEBUG C'est un l (sans param)" << std::endl;
		chan->changeModeL(this, client, std::string(1, sign) + "l", 0);
	}
	else
		std::cout << "DEBUG Mode inconnu : " << c << std::endl;
}
