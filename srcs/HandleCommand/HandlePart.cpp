/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 15:36:46 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"


void Server::verifIfCloseChannel(Channel* channel)
{
	if (channel && channel->isChannelEmpty() == 0)
	{
		std::string name = channel->getName();
		for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			if (*it == channel)
			{
				_channels.erase(it);
				break;
			}
		}
		delete channel;
	}
}



void Server::commandPart(Client* client, Channel* channel, std::string comment)
{
	if (channel->verifClientisInChannel(client))
	{
		std::string partMsg;
		if (!comment.empty())
			partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + " :" + comment + "\r\n";
		else
			partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + "\r\n";
		channel->ChannelSend(partMsg, client);
		client->send_msg(partMsg);
		channel->removeUser(client->getFd());
        channel->removeOperator(client);
		if (channel->affectNextOperator() == 1)
		{
			verifIfCloseChannel(channel);
			channel = NULL;
		}
    }
	else
		sendError(client->getFd(), "442", channel->getName(), "You're not on that channel");
}

void Server::handlePART(Client* client, const Message& msg)
{
    if (msg.params.empty()) 
	{
        sendError(client->getFd(), "461", "PART", "Not enough parameters");
        return;
    }

    std::string channelName = msg.params[0];
    Channel* chan = findChannel(channelName);

    if (!chan) {
        sendError(client->getFd(), "403", channelName, "No such channel");
        return;
    }
    std::string comment = msg.trailing;
    commandPart(client, chan, comment);
}
