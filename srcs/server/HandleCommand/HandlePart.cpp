/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 12:17:37 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 15:01:00 by smolines         ###   ########.fr       */
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
		if (comment == "")
            std::string partMsg = ":" + client->getPrefix() + " PART " + chan->getName() + " :" + comment + "\r\n";
		else
		    std::string partMsg = ":" + client->getPrefix() + " PART " + chan->getName() + "\r\n";
        channel->ChannelSend(partMsg, client);
        std::cout << "[DEBUG] commande PART -> left channel " << channel->getName() << "\n";
		channel->removeUser(client->getFd());
        channel->removeOperator(client);
		verifIfCloseChannel(Channel* channel);
    }
	else
	{
			sendError(client->getFd(), "442", channel->getName(), "You're not on that channel");
			// message a client uniquement 
	        std::cout << "[DEBUG] commande PART : you are not on this channel" << std::endl;
	}
}

