/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/04 16:35:11 by smolines         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

// commande PART dans server

void Server::verifIfCloseChannel(Channel* channel)
{
	if (channel->isChannelEmpty() == 0)
		delete channel;
	std::cout << "[DEBUG] channel" << channel->getName() << "has been deleted" << std::endl;
}


void Server::commandPart(Client* client, Channel* channel, std::string comment)
{
	if (channel->verifClientisInChannel(client))
	{
        std::cout << "[DEBUG] commande PART -> client is present in channel!\n";
    	   //message a tous les membres du channel + le client qui vient de partir;
		std::string partMsg;
		if (comment.empty())
            partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + " :" + comment + "\r\n";
		else
		    partMsg = ":" + client->getPrefix() + " PART " + channel->getName() + "\r\n";
        channel->ChannelSend(partMsg, client);
        std::cout << "[DEBUG] commande PART -> left channel " << channel->getName() << "\n";
		channel->removeUser(client->getFd());
        channel->removeOperator(client);
		verifIfCloseChannel(channel);
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
    if (msg.params.empty()) {
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
    std::cout << "--------- HANDLE PART ---------" << std::endl;
    std::cout << "PART = " << comment << std::endl;
    std::cout << "Channel = " << channelName << std::endl;
    std::cout << "-------------------------------" << std::endl;
    commandPart(client, channel, comment);
}
