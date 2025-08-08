/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleJoin.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 20:20:42 by zmogne            #+#    #+#             */
/*   Updated: 2025/08/08 15:46:38 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Utils.hpp"


void Server::handleSingleJoin(Client *client, const std::string &channelName, const std::string &key)
{
	if (channelName.empty() || channelName[0] != '#')
	{
		sendError(client->getFd(), "403", channelName, "No such channel");
		return;
	}
	Channel* chan = findChannel(channelName);
	bool isNewChannel = false;
	if (!chan)
	{		
		Channel* newChannel = new Channel(channelName);
		_channels.push_back(newChannel);
		chan = newChannel;
		isNewChannel = true;
	
	}
	if (chan->verifClientisUser(client))
        return;
	if (chan->addUser(this, client, key) == false)
		return;
	sendJoinMsg(client, chan);
	sendTopic(client, chan);
	sendNameList(client, chan);
	if (isNewChannel)
	{
		std::string infoMsg = ":" + _server_name + " PRIVMSG " + chan->getName() +
							" :Channel " + channelName + " created on " + getCurrentDate();
		chan->ChannelSend(infoMsg, NULL);
	}
}

bool Server::parseJoin(const Message &msg, std::string &channel,
	std::string &key)
{
	channel.clear();
	key.clear();
	const std::string &p0 = msg.params[0];
	if (msg.params.empty())
		return (false);
	if (p0.empty() || p0[0] != '#')
		return (false);
	channel = p0;
	if (msg.params.size() >= 2) 
	{
		if (!msg.params[1].empty() && msg.params[1][0] == '#')
			key.clear();
		else 
			key = msg.params[1];
	}
	return (true);
}


void Server::handleJOIN(Client *client, const Message &msg)
{
	std::string channel, key, user;
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "JOIN", "Not enough parameters");
		return ;
	}
    if (!client->isRegistered())
	{
		sendError(client->getFd(), "451", client->getNickname(), "You have not registered");
        return;
    }
    if (!msg.prefix.empty() && toLower(msg.prefix) != toLower(client->getNickname()))    
    {
        sendError(client->getFd(), "451", client->getNickname(), "prefix different from user nickname");
        return;
    }

    if (msg.params[0] == "0" || msg.params[0] == "#0")
	{
        leaveAllChannels(client);
		return;
	}
    std::vector<std::string> channels = splitComma(msg.params[0]);
	std::vector<std::string> keys;
    if (msg.params.size() > 1)
		keys = splitComma(msg.params[1]);
	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string chan = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";
		std::string tmpChannel, tmpKey;
		Message msg_chan("JOIN " + chan + " :" + key);
		if (!parseJoin(msg_chan,tmpChannel, tmpKey)) 
		{
			sendError(client->getFd(), "403", chan, "No such channel");
			continue;
		}
        if (ClientChannelCount(client) >= 10)
        {
            sendError(client->getFd(), "405", channels[i], "You have joined too many channels");
            return;
        }
		handleSingleJoin(client, chan, key);
	}
}

void Server::leaveAllChannels(Client *client) 
{
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); )
    {
        Channel* chan = *it;
        if (chan->verifClientisUser(client) || chan->verifClientisOperator(client))
        {
            chan->removeUser(client->getFd());
            chan->removeOperator(client);

            std::string partMsg = ":" + client->getPrefix() + " PART " + chan->getName() + " :Leaving all channels\r\n";
            chan->ChannelSend(partMsg, client);
			client->send_msg(partMsg);
        }
        int result = chan->getClientCount();
        if (result == 0)
            verifIfCloseChannel(chan);
        else
            ++it;
    }
}


void Server::sendNameList(Client* client, Channel* chan)
{
	std::string memberList;

	const std::vector<Client*>& operators = chan->getOperators();
	for (size_t i = 0; i < operators.size(); ++i)
	{
		memberList += "@" + operators[i]->getNickname();
		memberList += " ";
	}
	const std::vector<Client*>& users = chan->getUsers();
	for (size_t i = 0; i < users.size(); ++i)
	{
		Client* user = users[i];
		if (std::find(operators.begin(), operators.end(), user) != operators.end())
			continue;
		memberList += user->getNickname();
		memberList += " ";
	}
	if (!memberList.empty() && memberList[memberList.size() - 1] == ' ')
		memberList.erase(memberList.size() - 1);
	std::string namesMsg = ":" + _server_name + " 353 " + client->getNickname() +
		" = " + chan->getName() + " :" + memberList + "\r\n";
	client->send_msg(namesMsg);
	std::string endNamesMsg = ":" + _server_name + " 366 " + client->getNickname() +
		" " + chan->getName() + " :End of /NAMES list\r\n";
	client->send_msg(endNamesMsg);
}



void Server::sendTopic(Client* client, Channel* chan)
{
	if (!chan->getTopicName().empty())
	{
		std::string topicMsg = ":" + _server_name + " 332 " + client->getNickname() +
			" " + chan->getName() + " :" + chan->getTopicName() + "\r\n";
		client->send_msg(topicMsg);
	}
	else
	{
		std::string noTopicMsg = ":" + _server_name + " 331 " + client->getNickname() +
			" " + chan->getName() + " :No topic is set\r\n";
		client->send_msg(noTopicMsg);
	}
}

void Server::sendJoinMsg(Client* client, Channel* chan)
{
	std::string joinMsg = ":" + client->getPrefix() + " JOIN :" + chan->getName() + "\r\n";
	chan->ChannelSend(joinMsg, NULL);
}
