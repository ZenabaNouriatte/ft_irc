/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   otherCommand.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:14:57 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/05 17:43:31 by smolines         ###   ########.fr       */
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

// void Channel::commandTopic(Server *server, Client *client, std::string topic)
// {
// 	std::cout << "DEBUG Dans fonction COMMANDTOPIC" << std::endl;
	
// 	// if topic est vide : afficher le topic
// 	if (_topicName.empty())
// 	{
// 		// message recu par tous les utilisateurs du canal y compris le demandeur :
// 		//:<nick client>!user@host TOPIC <#channel> :<topic>
// 		// pas de message specifique envoye uniquement au demandeur
// 		std::cout << "DEBUG Aucun topic defini" << std::endl;
// 	}
// 	else if (!topic.empty() && _topicRestriction == true)
// 	{
// 		std::cout << "DEBUG  Topic restriction" << std::endl;
// 		if (verifClientisOperator(client) == true)
// 		{
// 			_topicName = topic;
// 			// message recu par tous les utilisateurs du canal y compris le demandeur :
// 			//:<nick client>!user@host TOPIC <#channel> :<topic>
// 			// pas de message specifique envoye uniquement au demandeur
// 			std::cout << "DEBUG commtPOPIC : message nouveau topic" << std::endl;
// 		}
// 		else
// 		{
// 			server->sendError(client->getFd(), "482", this->_name,
// 				"You're not channel operator");
// 			std::cout << "DEBUG commtPOPIC : client not an operator" << std::endl;
// 		}
// 	}
// 	else
// 	{
// 		std::cout << "DEBUG  Pas de Topic restriction et pas vide" << std::endl;
// 		_topicName = topic;
// 		// message recu par tous les utilisateurs du canal y compris le demandeur :
// 		//:<nick client>!user@host TOPIC <#channel> :<topic>
// 		// pas de message specifique envoye uniquement au demandeur
// 	}
// }

void Channel::commandTopic(Server *server, Client *client, std::string topic)
{
	std::cout << "[DEBUG][TOPIC] Entrée dans commandTopic()\n";

	if (topic.empty())
	{
		std::cout << "[DEBUG][TOPIC] Aucun nouveau topic fourni → affichage du topic actuel\n";
		if (_topicName.empty())
		{
			std::cout << "[DEBUG][TOPIC] Aucun topic défini pour le channel '" << _name << "'\n";
			std::string msg = ":" + server->getServerName() + " 331 " + client->getNickname() + " " + _name + " :No topic is set\r\n";
			client->send_msg( msg);
		}
		else
		{
			std::cout << "[DEBUG][TOPIC] Topic actuel = '" << _topicName << "'\n";
			std::string msg = ":" + server->getServerName() + " 332 " + client->getNickname() + " " + _name + " :" + _topicName + "\r\n";
			client->send_msg(msg);
		}
		return;
	}

	std::cout << "[DEBUG][TOPIC] Un nouveau topic est fourni : '" << topic << "'\n";

	if (_topicRestriction)
	{
		std::cout << "[DEBUG][TOPIC] La restriction de topic est activée → vérification opérateur\n";
		if (!verifClientisOperator(client))
		{
			std::cout << "[DEBUG][TOPIC] Client n'est pas opérateur → refus de changer le topic\n";
			server->sendError(client->getFd(), "482", _name, "You're not channel operator");
			return;
		}
		std::cout << "[DEBUG][TOPIC] Client est opérateur → changement autorisé\n";
	}

	_topicName = topic;

	std::cout << "[DEBUG][TOPIC] Nouveau topic : [" 
		<< _topicName << "] défini pour le channel " 
		<< _name << "   - ptr: " << this << std::endl;
	std::string topicMsg = ":" + client->getPrefix() + " TOPIC " + _name + " :" + topic + "\r\n";
	ChannelSend(topicMsg, NULL); // Diffusion à tous les membres du channel
}


/*========================== KICK ============================*/

// client : celui qui envoie la commande
// cible : nickname utilise dans la commande
//chan2 : channel demande dans la commande
void Channel::commandKick(Server *server, Client *client, Client *cible,
	std::string comment)
{
	if (client == cible)
		return;

		/////////////////////////////
	if (!verifClientisInChannel(client))
	{
		std::cout << "DEBUG commKick : le client pas dans channel" << std::endl;
		server->sendError(client->getFd(), "442", this->_name, "You're not on that channel");
		return;
	}
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible))
		{
			if (comment.empty())
			{
				std::cout << "DEBUG commKick : message confirmation sans comment" << std::endl;
				// message envoye a tous y compris client et cible
				// message confirmation : :<nick>!user@host KICK #channel cible
				std::string kickMsg = ":" + client->getPrefix() + " KICK " + _name + cible->getNick() + "\r\n";
				std::cout << "DEBUG commKick : message" << kickMsg << std::endl;
				ChannelSend(kickMsg, NULL); // Diffusion à tous les membres du channel
			}
			else
			{
			std::cout << "DEBUG commKick : message confirmation avec comment" << std::endl;
			// message envoye a tous y compris client et cible
			// message confirmation : :<nick>!user@host KICK #channel cible :<comment>
			std::string kickMsg = ":" + client->getPrefix() + " KICK " + _name + " " + cible->getNick() + " " +comment +"\r\n";
			std::cout << "DEBUG commKick : message avec comment" << kickMsg << std::endl;
			ChannelSend(kickMsg, NULL); // Diffusion à tous les membres du channel
			}
			removeUser(cible->getFd());
			removeInvited(cible);
			removeOperator(cible);			
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
			// message:	:<Nick client>!user@host INVITE <nick cible> :#channel : recu uniquement par cible
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
	{
		std::cout << "DEBUG ADDUSER ajouter operator" << std::endl;
		_operators.push_back(user);
	}
	// envoi des messages lies a la creation du channel ???
	// message: :<nick>!<user>@<host> JOIN :#channel
	//:<serveur> MODE #channel +o <nick>
	// :<serveur> 324 <nick> #channel +o
	// :<serveur> 353 <nick> = #channel :<nick>
	// :<serveur> 366 <nick> #channel :End of NAMES list
	// :<serveur> 331 <nick> #channel42 :No topic is set
	else
	{
		std::cout << "DEBUG ADDUSER ajouter user" << std::endl;
		_users.push_back(user);
	}
	// message: :<nick>!<user>@<host> JOIN :#channel
	// message : liste des modes du channel
	// message : liste des clients du channel
	// message : topic du channel
	return ;
}
