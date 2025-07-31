/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:04:03 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 14:17:30 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

/*===================== Constructor =====================*/
Channel::Channel(std::string name)
{
	this->_name = name;
	this->_inviteOnly = 0;
	this->_limit = 0;
	this->_topic = false;
	this->_topicRestriction = false;
	this->_hasLimit = false;
	this->_key = false;
	std::cout << GREEN << BOLD << "Channel [" << this->_name << "] successfully created" << RESET << std::endl;
	return ;
}

Channel::Channel(std::string name, std::string topicName)
{
	this->_topicName = topicName;
	this->_name = name;
	this->_inviteOnly = 0;
	this->_limit = 0;
	this->_topic = true;
	this->_topicRestriction = false;
	this->_hasLimit = false;
	this->_key = false;
	std::cout << GREEN << BOLD << "Channel [" << this->_name << "] successfully created" << RESET << std::endl;
	return ;
}

/*===================== Destructor =====================*/

Channel::~Channel()
{
	return ;
}

/*===================== GETTER =====================*/

bool Channel::getTopic()
{
	return (this->_topic);
}
std::string Channel::getName()
{
	return (this->_name);
}
std::string Channel::getTopicName()
{
	return (_topicName);
}
std::string Channel::getPassWord(){return (this->_password);}
bool Channel::getTopicRestriction(){return (this->_topicRestriction);}
bool Channel::getInviteOnly(){return (this->_inviteOnly);}
int Channel::getLimit(){return (this->_limit);}
bool Channel::getHasLimit(){return (this->_hasLimit);}
bool Channel::getKey(){return (this->_key);}
std::vector<Client *> Channel::getUsers(){return (this->_users);}
std::vector<Client *> Channel::getOperators(){return (this->_operators);}

int Channel::getClientCount() const
{
	std::set<Client *> uniqueClients; // stl set pas de doublon

	// Ajouter tous les user
	uniqueClients.insert(_users.begin(), _users.end());
	// Ajouter operator (pour poas ajouter deux fois)
	uniqueClients.insert(_operators.begin(), _operators.end());
	return (uniqueClients.size());
}

/*===================== SETTER =====================*/

void Channel::setName(std::string name)
{
	this->_name = name;
	return ;
}

void Channel::setTopicName(std::string topic)
{
	this->_topicName = topic;
	setTopic(true);
	return ;
}

void Channel::setPassWord(std::string psswd)
{
	this->_password = psswd;
	setKey(true);
	return ;
}

void Channel::setTopicRestriction(bool rectriction)
{
	this->_topicRestriction = rectriction;
	return ;
}

void Channel::setInviteOnly(bool invite)
{
	this->_inviteOnly = invite;
	return ;
}

void Channel::setTopic(bool topic)
{
	this->_topic = topic;
	return ;
}

void Channel::setLimit(int nb)
{
	this->_limit = nb;
	setHasLimit(true);
	return ;
}

void Channel::setHasLimit(bool limit)
{
	this->_hasLimit = limit;
	return ;
}

void Channel::setKey(bool key)
{
	this->_key = key;
	return ;
}

/*===================== METHODE =====================*/

int Channel::isChannelEmpty() const // renvoie le nombre d'utilisateurs connectes au channel : user + operator
{
	return (_users.size() + _operators.size());
}

void Channel::addOperator(Client *client)
// a la creation : operator = user numero 1. Si user#1 quitte : definir un nouvel operator
// ajouter un op
{
	if (verifClientisUser(client) == true)
		userToOperator(client);
	else if (verifClientisOperator(client) == false)
		_operators.push_back(client);
}

void Channel::addInvited(Client *client)
// ajouter un nick a la liste des invites
{
	if (verifClientisInvited(client) == false)
		_isInvited.push_back(client);
}

void Channel::userToOperator(Client *user)
{
	_operators.push_back(user);
	std::vector<Client *>::iterator it = std::find(_users.begin(), _users.end(),
			user);
	if (it != _users.end())
		_users.erase(it);
}

void Channel::operatorToUser(Client *user)
{
	_users.push_back(user);
	std::vector<Client *>::iterator it = std::find(_operators.begin(),
			_operators.end(), user);
	if (it != _operators.end())
		_operators.erase(it);
}

bool Channel::isValidChannelPW(const std::string &password)
{
	char	c;

	if (password.empty() || password.size() > 23)
		return (false);
	for (size_t i = 0; i < password.size(); ++i)
	{
		c = password[i];
		if (std::isspace(static_cast<unsigned char>(c)) || c == ':' || c == ',')
			return (false);
	}
	return (true);
}
