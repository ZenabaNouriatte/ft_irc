/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:57:45 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 14:01:40 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*================== ERROR =======================*/

void Server::sendError(int fd, const std::string &code,
	const std::string &target, const std::string &message)
{
	std::string full = ":" + _server_name + " " + code + " " + target + " :"
		+ message + "\n";
	send(fd, full.c_str(), full.size(), 0);
}

void Server::sendError2(int fd, const std::string& code, const std::string& target, const std::string& targetBis, const std::string& message)
{
    std::string full = ":" + _server_name + " " + code + " " + target + " " + targetBis +" :" + message + "\n";
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
		Client *client = it->second; // valeur dans la map
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
            std::string prefix = ":" + sender->getPrefix(); // pas de prefix dans client
            std::string irc_line = prefix + " PRIVMSG " + target + " :" + message + "\r\n";
            dest->send_msg(irc_line);
            return true;
        }
    }
    return false;
}