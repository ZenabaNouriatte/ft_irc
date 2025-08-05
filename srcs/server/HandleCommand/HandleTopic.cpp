/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleTopic.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 09:58:55 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/05 14:18:14 by smolines         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleTOPIC(Client *client, const Message &msg)
{
    std::cout << "[DEBUG] HANDLE TOPIC" << std::endl;
    if (msg.params.empty())
//    (msg.params[0].empty())
    {
        std::cout << "[DEBUG] MSG.PARAM EMPTY" << std::endl;
        sendError(client->getFd(), "461", "TOPIC", "Not enough parameters");
        return;
    }
    
    std::string channelName = msg.params[0];
    Channel *chan = findChannel(channelName);
    if (!chan)
    {
        sendError(client->getFd(), "403", channelName, "No such channel");
        return;
    }
    if (msg.trailing.empty())
    {
        //sendError(client->getFd(), "461", "TOPIC", "Not enough parameters");
        std::cout << "[DEBUG] MSG.PARAM EMPTY && TRAILING EMPTY" << std::endl;
        chan->commandTopic(this,client,"");
        return;
    }
    if (!chan->verifClientisInChannel(client))
    {
        sendError(client->getFd(), "442", channelName, "You're not on that channel");
        return;
    }

    std::string topic;
    
    if (!msg.trailing.empty())
    {
        topic = msg.trailing;
    }
    else
    {
        std::cout << "[DEBUG] Dans else" << topic << std::endl;
        topic = "";
    }

    std::cout << "[DEBUG] topic = " << topic << std::endl;
    chan->commandTopic(this,client, topic);
}
