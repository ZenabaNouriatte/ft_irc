/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandlePart.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 16:08:18 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/04 16:23:48 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"

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
