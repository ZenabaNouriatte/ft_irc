/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleTopic.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 09:58:55 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 10:07:27 by cschmid          ###   ########.fr       */
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

    if (!chan->verifClientisInChannel(*client))
    {
        sendError(client->getFd(), "442", channelName, "You're not on that channel");
        return;
    }

    std::string topic;
    if (msg.params.size() > 1)
        topic = msg.params[1];
    else
        topic = "";

    std::cout << "[DEBUG] topic = " << topic << std::endl;
    //   chan->commandTopic(client, topic);
}
