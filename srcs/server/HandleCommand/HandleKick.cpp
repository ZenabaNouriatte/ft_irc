/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleKick.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 09:05:39 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 09:50:43 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleKICK(Client *client, const Message &msg)
{
    if (msg.params.size() < 2)
    {
        sendError(client->getFd(), "461", "KICK", "Not enough parameters");
        return;
    }

    std::string channelName = msg.params[0];
    std::string targetNick = msg.params[1];

    std::string reason;
    if (msg.trailing.empty())
        reason = "No reason";
    else
        reason = msg.trailing;

    Channel *chan = findChannel(channelName);
    if (!chan)
    {
        sendError(client->getFd(), "403", channelName, "No such channel");
        return;
    }

    Client *target = chan->findClientByNick(targetNick);
    if (!target)
    {
        sendError(client->getFd(), "441", targetNick, "They aren't on that channel");
        return;
    }
    std::cout << "[DEBUG] Target = " << targetNick << std::endl;
    std::cout << "[DEBUG] reason = " << reason << std::endl;
    //chan->commandKick(client, target, reason);
}
