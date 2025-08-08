/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:07:32 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 15:46:48 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

/*========================== REMOVE ============================*/

int Channel::affectNextOperator()
{
	if (_operators.size() == 0)
	{
		if (!_users.empty() && _operators.empty()){
 		   Client* firstUser = _users.front();
		   userToOperator(firstUser);
		}
		else
			return 1;
	}
		return 0;
}

void Channel::removeOperator(Client *user)
{
	std::vector<Client *>::iterator it = std::find(_operators.begin(),
			_operators.end(), user);
	if (it != _operators.end())
		_operators.erase(it);
}

void Channel::removeInvited(Client *user)
{
	std::vector<Client *>::iterator it = std::find(_isInvited.begin(),
			_isInvited.end(), user);
	if (it != _isInvited.end())
		_isInvited.erase(it);
}

void Channel::removeUser(int fd)
{
	for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			_users.erase(it);
			break ;
		}
	}
	return ;
}

/*========================== VERIF ============================*/

bool Channel::verifClientisInChannel(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(),
			client) != _operators.end() || std::find(_users.begin(),
			_users.end(), client) != _users.end())
		return (true);
	return (false);
}

bool Channel::verifClientisOperator(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(), client) != _operators.end())
		return (true);
	return (false);
}

bool Channel::verifClientisUser(Client *client)
{
	return (std::find(_users.begin(), _users.end(), client) != _users.end());
	return (false);
}

bool Channel::verifClientisInvited(Client *client)
{
	return (std::find(_isInvited.begin(), _isInvited.end(),
			client) != _isInvited.end());
}


void Channel::ChannelSend(const std::string &message, Client *sender)
{
	for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (*it != sender)
			(*it)->send_msg(message);
	}
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it != sender)
			(*it)->send_msg(message);
	}
}


std::string getCurrentDate()
{
    time_t now = time(0);
    char buf[80];
    struct tm tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %b %d %Y %H:%M:%S", &tstruct);
    return std::string(buf);
}
