/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otherCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:14:57 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 17:20:30 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>



/*========================== TOPIC ============================*/

void Channel::changeTopic(Server *server, Client *client, std::string topic)
{
	if (_topicRestriction == true && verifClientisOperator(client) == false)
		server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
	else
	{
		this->_topicName = topic;
		this->_topic = true;
		std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +t" + "\r\n";
		ChannelSend(modeMsg, NULL); 
	}
}

void Channel::commandTopic(Server *server, Client *client, std::string topic)
{

	if (topic.empty())
	{
		if (_topicName.empty())
		{
			std::string msg = ":" + server->getServerName() + " 331 " + client->getNickname() + " " + _name + " :No topic is set\r\n";
			client->send_msg( msg);
		}
		else
		{
			std::string msg = ":" + server->getServerName() + " 332 " + client->getNickname() + " " + _name + " :" + _topicName + "\r\n";
			client->send_msg(msg);
		}
		return;
	}
	
	if (_topicRestriction)
	{
		if (!verifClientisOperator(client))
		{
			server->sendError(client->getFd(), "482", _name, "You're not channel operator");
			return;
		}
	}

	_topicName = topic;
	std::string topicMsg = ":" + client->getPrefix() + " TOPIC " + _name + " :" + topic + "\r\n";
	ChannelSend(topicMsg, NULL);
}


/*========================== KICK ============================*/

void Channel::commandKick(Server *server, Client *client, Client *cible,
	std::string comment)
{
	if (client == cible)
		return;
	if (!verifClientisInChannel(client))
	{
		server->sendError(client->getFd(), "442", this->_name, "You're not on that channel");
		return;
	}
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible))
		{
			if (comment.empty())
			{
				std::string kickMsg = ":" + client->getPrefix() + " KICK " + _name + cible->getNick() + "\r\n";
				ChannelSend(kickMsg, NULL);
			}
			else
			{
				std::string kickMsg = ":" + client->getPrefix() + " KICK " + _name + " " + cible->getNick() + " " +comment +"\r\n";
				ChannelSend(kickMsg, NULL);
			}
			removeUser(cible->getFd());
			removeInvited(cible);
			removeOperator(cible);			
		}
		else
			server->sendError2(client->getFd(), "441", cible->getNick(), this->_name, "They aren't on that channel");
	}
	else
		server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
}

/*========================== INVIT ============================*/

// inviter une personne deja invitee ne cree pas d'erreur et le message de confirmnation est envoye
void Channel::commandInvite(Server *server, Client *client, Client *cible)
{
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible) == true)
			server->sendError(client->getFd(), "443", cible->getNick(), "is already on channel");
		else
		{
			addInvited(cible);
			std::string inviteMsg = ":" + client->getPrefix() + " INVITE " + cible->getNick() + " :" + this->_name;
			cible->send_msg(inviteMsg); 

			std::string confirmMsg = ":" + server->getServerName() + " 341 " + client->getNick() + " " + cible->getNick() + " " + this->_name;
			client->send_msg(confirmMsg);
		}
	}
	else
		server->sendError(client->getFd(), "482", this->_name,	"You're not channel operator");
}


/*========================== ADDUSER ============================*/

bool Channel::addUser(Server *server, Client *user, std::string key)
{
	if (verifClientisInChannel(user) == true)
	{
		server->sendError2(user->getFd(), "443", user->getNick(), this->_name, "is already on channel");
		return (0);
	}
	else if (_inviteOnly == true && verifClientisInvited(user) == false)
	{
		server->sendError(user->getFd(), "473", this->_name, "Cannot join channel (+i)");
		return (0);
	}
	else if (_hasLimit == true && (_users.size() + _operators.size()) == _limit)
	{
		server->sendError(user->getFd(), "471", this->_name, "Cannot join channel (+l)");
		return (0);
	}
	else if (this->_key && (this->_password != key || key.empty()))
	{
		server->sendError(user->getFd(), "475", this->_name,	"Cannot join channel (+k)");
		return (0);
	}
	else if (_operators.empty())
	{
		_operators.push_back(user);
		return (1);
	}
	else
	{
		_users.push_back(user);
		return (1);
	}
}



