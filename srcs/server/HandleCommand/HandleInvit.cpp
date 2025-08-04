/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleInvit.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 09:42:56 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/04 13:01:43 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

void Server::handleINVIT(Client *client, const Message &msg)
{
    if (msg.params.size() < 2) {
        sendError(client->getFd(), "461", "INVITE", "Not enough parameters");
        return;
    }

    std::string targetNick = msg.params[0];
    std::string channelName = msg.params[1];

    Channel *chan = findChannel(channelName);
    if (!chan) {
        sendError(client->getFd(), "403", channelName, "No such channel");
        return;
    }

    Client *target = findClient(targetNick);
    if (!target) {
        sendError(client->getFd(), "401", targetNick, "No such nick");
        return;
    }
    std::cout << "[DEBUG] Target = " << targetNick << std::endl;
    chan->commandInvite(this,client, target);
}
