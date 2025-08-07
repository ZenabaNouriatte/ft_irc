/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/07 20:46:42 by zmogne           ###   ########.fr       */
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
		std::cout << "[DEBUG] channel " << name << " has been deleted" << std::endl;
	}
}



void Server::commandPart(Client* client, Channel* channel, std::string comment)
{
	if (channel->verifClientisInChannel(client))
	{
        std::cout << "[DEBUG] commande PART -> client is present in channel!\n";
    	   //message a tous les membres du channel + le client qui vient de partir;
		std::string partMsg;
		if (!comment.empty())
			partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + " :" + comment + "\r\n";
		else
			partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + "\r\n";
        //std::cout << "[DEBUG] commande PART -> left channel " << channel->getName() << "\n";
		channel->ChannelSend(partMsg, client);
		client->send_msg(partMsg);
		channel->removeUser(client->getFd());
        channel->removeOperator(client);
		if (channel->affectNextOperator() == 1)
		{
			verifIfCloseChannel(channel);
			channel = NULL;
			// std::cout << "[DEBUG] channel deleted" << channel->getName() << "has been deleted" << std::endl;
			//message ??
		}
		
		// A VOIR SI A AJOUTER DANS REMOVE OPERATOR
    }
	else
	{
			sendError(client->getFd(), "442", channel->getName(), "You're not on that channel");
			// message a client uniquement 
	        std::cout << "[DEBUG] commande PART : you are not on this channel" << std::endl;
	}
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
    std::string comment = msg.trailing; //DEBUG 
    std::cout << "DEBUG --------- HANDLE PART ---------" << std::endl;
    std::cout << "DEBUG PART = " << comment << std::endl;
    std::cout << "DEBUG Channel = " << channelName << std::endl;
    std::cout << "-------------------------------" << std::endl;
    commandPart(client, chan, comment);
}
