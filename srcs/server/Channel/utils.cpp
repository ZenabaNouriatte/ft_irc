/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:07:32 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/04 13:18:23 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Channel.hpp>

/*========================== REMOVE ============================*/

void Channel::removeOperator(Client *user)
{
	std::vector<Client *>::iterator it = std::find(_operators.begin(),
			_operators.end(), user);
	if (it != _operators.end())
		_operators.erase(it);
}

void Channel::removeInvited(Client *user)
{
	std::vector<Client *>::iterator it = std::find(_isInvited.begin(),
			_isInvited.end(), user);
	if (it != _isInvited.end())
		_isInvited.erase(it);
}

void Channel::removeUser(int fd)
{
	for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			_users.erase(it);
			break ;
		}
	}
	return ;
}

/*========================== VERIF ============================*/

bool Channel::verifClientisInChannel(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(),
			client) != _operators.end() || std::find(_users.begin(),
			_users.end(), client) != _users.end())
		return (true);
	return (false);
}

bool Channel::verifClientisOperator(Client *client)
{
	if (std::find(_operators.begin(), _operators.end(), client) != _operators.end())
		return (true);
	return (false);
}

bool Channel::verifClientisUser(Client *client)
{
	return (std::find(_users.begin(), _users.end(), client) != _users.end());
	return (false);
}

bool Channel::verifClientisInvited(Client *client)
{
	return (std::find(_isInvited.begin(), _isInvited.end(),
			client) != _isInvited.end());
}

/*========================== PRINT / DEBUG ============================*/

void Channel::printUsers() const
{
	for (std::vector<Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it)
		std::cout << "       - " << *it << "\n";
	for (std::vector<Client *>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
		std::cout << "       - (op) " << *it << "\n";
}

//---- DEBUG ZENABA
// void Channel::ChannelSend(const std::string &message, Client *sender)
// {
// 	// user
// 	for (std::vector<Client *>::iterator it = _users.begin(); it != _users.end(); ++it)
// 	{
// 		if (*it != sender)
// 			(*it)->send_msg(message);
// 	}

// 	// operator
// 	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
// 	{
// 		// pouor ne pas aussi envoyer a user donc 2 fois
// 		if (*it != sender && std::find(_users.begin(), _users.end(), *it) == _users.end())
// 			(*it)->send_msg(message);
// 	}
// }

void Channel::ChannelSend(const std::string &message, Client *sender)
{
    std::set<Client*> recipients;

    // Ajouter les utilisateurs (sauf sender)
    for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (*it != sender)
            recipients.insert(*it);
    }

    // Ajouter les opérateurs (sauf sender)
    for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
    {
        if (*it != sender)
            recipients.insert(*it); // set évite les doublons automatiquement
    }

    // Envoyer le message à chaque destinataire unique
    for (std::set<Client*>::iterator it = recipients.begin(); it != recipients.end(); ++it)
    {
        (*it)->send_msg(message);
    }
}


void Channel::printClientVectors() const
{
	std::cout << "  [_users] Clients:\n";
	for (std::vector<Client *>::const_iterator it = _users.begin(); it != _users.end(); ++it)
		std::cout << "    - ptr: " << *it << " (" << (*it)->getNickname() << ")\n";

	std::cout << "  [_operators] Clients:\n";
	for (std::vector<Client *>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
		std::cout << "   - ptr: " << *it << " (" << (*it)->getNickname() << ")\n";
}
