/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleTopic.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 09:58:55 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 15:37:09 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleTOPIC(Client *client, const Message &msg)
{
    if (msg.params.empty())
    {
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
        topic = msg.trailing;
    else
        topic = "";
    chan->commandTopic(this,client, topic);
}
