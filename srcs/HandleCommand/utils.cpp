/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:57:45 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 15:37:18 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*================== ERROR =======================*/

void Server::sendError(int fd, const std::string &code,
	const std::string &target, const std::string &message)
{
	std::string full = ":" + _server_name + " " + code + " " + target + " :"
		+ message + "\r\n";
    std::cout << "Client [" << fd << "] >> " << full << std::endl;
	send(fd, full.c_str(), full.size(), 0);
}

void Server::sendError2(int fd, const std::string& code, const std::string& target, const std::string& targetBis, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " " + targetBis +" :" + message + "\r\n";
    send(fd, full.c_str(), full.size(), 0);
}

/*=================== FIND =======================*/

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
		Client *client = it->second;
		if (client && client->getNickname() == nickname)
			return (client);
	}
	return (NULL);
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

/*=================== Message =======================*/

bool Server::MsgToChannel(Client* sender, const std::string& channelName, const std::string& message)
{
    Channel* channel = findChannel(channelName);
    if (!channel)
    {
        sendError(sender->getFd(), "403", channelName, "No such channel");
        return false;
    }
    if (!channel->verifClientisUser(sender) && !channel->verifClientisOperator(sender))
    {
        sendError(sender->getFd(), "442", channelName, "You're not on that channel");
        return false;
    }

    std::string irc_line = ":" + sender->getPrefix() + " PRIVMSG " + channelName + " :" + message + "\r\n";

    channel->ChannelSend(irc_line, sender);

    return true;
}

bool Server::PvMsgToUser(Client* sender, const std::string& target, const std::string& message)
{
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        Client* dest = it->second;
        if (dest->getNickname() == target)
        {
            const std::string& nick = sender->getNickname();
            const std::string& user = sender->getUsername();
            const std::string  host = "localhost";      
            std::string line = ":" + nick + "!~" + user + "@" + host
                             + " PRIVMSG " + target + " :" + message + "\r\n";

            dest->send_msg(line);   
            return true;
        }
    }
    return false;
}

bool isValidNickname(const std::string& nick) 
{
    if (nick.empty() || nick.length() > 15)
        return false;
    if (!std::isalpha(nick[0]))
        return false;
    for (size_t i = 1; i < nick.size(); ++i) 
    {
        char c = nick[i];
        if (!std::isalnum(c) &&
            c != '-' && c != '[' && c != ']' && c != '\\' &&
            c != '`' && c != '^' && c != '{' && c != '}')
            return false;
    }
    return true;
}

std::string toLower(const std::string &str) 
{
    std::string lower;
    for (size_t i = 0; i < str.size(); ++i)
        lower += std::tolower(str[i]);
    return lower;
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