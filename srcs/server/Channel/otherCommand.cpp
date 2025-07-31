/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otherCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:14:57 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 14:17:50 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

/*========================== TOPIC ============================*/

void Channel::changeTopic(Server *server, Client *client, std::string topic)
{
	if (_topicRestriction == true && verifClientisOperator(client) == false)
	{
		std::cout << "DEBUG changeTopic : refus de changement" << std::endl;
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
	}
	else
	{
		this->_topicName = topic;
		this->_topic = true;
		// message: :<nick>!user@host MODE #channel +t
		// message :serveur 324 <nick> #channel -t
	}
}

void Channel::commandTopic(Server *server, Client *client, std::string topic)
{
	// if topic est vide : afficher le topic
	if (topic == "")
	{
		// message recu par tous les utilisateurs du canal y compris le demandeur :
		//:<nick client>!user@host TOPIC <#channel> :<topic>
		// pas de message specifique envoye uniquement au demandeur
		std::cout << "DEBUG commtPOPIC : message topic actuel" << std::endl;
	}
	else if (_topicRestriction == true)
	{
		if (verifClientisOperator(client) == true)
		{
			_topicName = topic;
			// message recu par tous les utilisateurs du canal y compris le demandeur :
			//:<nick client>!user@host TOPIC <#channel> :<topic>
			// pas de message specifique envoye uniquement au demandeur
			std::cout << "DEBUG commtPOPIC : message nouveau topic" << std::endl;
		}
		else
		{
			server->sendError(client->getFd(), "482", this->_name,
				"You're not channel operator");
			std::cout << "DEBUG commtPOPIC : client not an operator" << std::endl;
		}
	}
	else
	{
		_topicName = topic;
		// message recu par tous les utilisateurs du canal y compris le demandeur :
		//:<nick client>!user@host TOPIC <#channel> :<topic>
		// pas de message specifique envoye uniquement au demandeur
	}
}

/*========================== KICK ============================*/

// client : celui qui envoie la commande
// cible : nickname utilise dans la commande
void Channel::commandKick(Server *server, Client *client, Client *cible,
	std::string comment)
{
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible) == true)
		{
			removeUser(cible->getFd());
			removeInvited(cible);
			if (comment != "")
			{
				std::cout << "DEBUG commKick : message confirmation sans comment" << std::endl;
				// message envoye a tous y compris client et cible
				// message confirmation : :<nick>!user@host KICK #channel cible
			}
			else
				std::cout << "DEBUG commKick : message confirmation avec comment" << std::endl;
			// message envoye a tous y compris client et cible
			// message confirmation : :<nick>!user@host KICK #channel cible :<comment>
		}
		else
		{
			std::cout << "DEBUG commKick : cible not in channel" << std::endl;
			server->sendError2(client->getFd(), "441", cible->getNick(),
				this->_name, "They aren't on that channel");
		}
	}
	else
	{
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
		std::cout << "DEBUG commKick : client not an operator" << std::endl;
	}
}

/*========================== INVIT ============================*/

// inviter une personne deja invitee ne cree pas d'erreur et le message de confirmnation est envoye
void Channel::commandInvite(Server *server, Client *client, Client *cible)
{
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible) == true)
		{
			server->sendError(client->getFd(), "443", cible->getNick(),
				"is already on channel");
			std::cout << "DEBUG commInvite : deja dans channel" << std::endl;
		}
		else
		{
			addInvited(cible);
			// message:	:<Nick client>!user@host INVITE <nick cible> :#channel : recu uniquement par bob
			// message:	:server 341 <Nick client> <nicjk cible> #channel : recu uniquement par client
		}
	}
	else
	{
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
		std::cout << "DEBUG commInvite : client not an operator" << std::endl;
	}
}

/*========================== ADDUSER ============================*/

void Channel::addUser(Server *server, Client *user, std::string key)
{
	// si l'utilisateur est deja dans le channel (user ou chanop) :
	if (verifClientisInChannel(user) == true)
	{
		server->sendError2(user->getFd(), "443", user->getNick(), this->_name,
			"is already on channel");
		std::cout << "DEBUG ADDUSER user already in the channel" << std::endl;
	}
	// si l'utilisateur est deja connecte a 10 channel /
	// DEBUG(client->getFd(), "405", "channel name", "You have joined too many channels");
	else if (_inviteOnly == true && verifClientisInvited(user) == false)
	{
		std::cout << "DEBUG ADDUSER channel on invite mode only" << std::endl;
		server->sendError(user->getFd(), "473", this->_name,
			"Cannot join channel (+i)");
	}
	else if (_hasLimit == true && (_users.size() + _operators.size()) < _limit)
	{
		std::cout << "DEBUG ADDUSER Too much users in this channel" << std::endl;
		server->sendError(user->getFd(), "471", this->_name,
			"Cannot join channel (+l)");
	}
	else if (this->_key && this->_password != key)
	{
		server->sendError(user->getFd(), "475", this->_name,
			"Cannot join channel (+k)");
		std::cout << "DEBUG ADDUSER bad password" << std::endl;
	}
	else if (_operators.empty())
		_operators.push_back(user);
	// envoi des messages lies a la creation du channel ???
	// message: :<nick>!<user>@<host> JOIN :#channel
	//:<serveur> MODE #channel +o <nick>
	// :<serveur> 324 <nick> #channel +o
	// :<serveur> 353 <nick> = #channel :<nick>
	// :<serveur> 366 <nick> #channel :End of NAMES list
	// :<serveur> 331 <nick> #channel42 :No topic is set
	else
		_users.push_back(user);
	// message: :<nick>!<user>@<host> JOIN :#channel
	// message : liste des modes du channel
	// message : liste des clients du channel
	// message : topic du channel
	return ;
}
