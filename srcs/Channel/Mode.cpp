/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:10:02 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/08 17:06:31 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <Channel.hpp>

void Channel::changeModeI(Server *server, Client *client, std::string arg)
{
	if (verifClientisOperator(client) == true)
	{
		if (arg == "-i")
		{
			this->_inviteOnly = false;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -i\r\n";
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -i" + "\r\n";
			client->send_msg(Msg);
		}
		else if (arg == "+i")
		{
			this->_inviteOnly = true;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +i\r\n";
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +i" + "\r\n";
			client->send_msg(Msg);
		}
	}
	else
		server->sendError(client->getFd(), "482", this->_name,"You're not channel operator");
}

void Channel::changeModeT(Server *server, Client *client, std::string arg)
{
	if (verifClientisOperator(client) == true)
	{
		if (arg == "+t")
		{
			this->_topicRestriction = true;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +t\r\n";
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +t" + "\r\n";
			client->send_msg(Msg);
		}
		if (arg == "-t")
		{
			this->_topicRestriction = false;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -t\r\n";
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -t" + "\r\n";
			client->send_msg(Msg);
		}
	}
}

void Channel::changeModeK(Server *server, Client *client, std::string arg,
	std::string key)
{
	if (verifClientisOperator(client) == true)
	{
		if (arg == "+k")
		{
			if (this->_key == true)
				server->sendError(client->getFd(), "467", this->_name,"Channel key already set");
			else if (this->_key == false && isValidChannelPW(key) == true)
			{
				this->_password = key;
				this->_key = true;
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +k " + key + "\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +k " + key + "\r\n";
				client->send_msg(Msg);
			}
			else
				server->sendError(client->getFd(), "475", this->_name, "Cannot join channel (+k)");
		}
		else if (arg == "-k")
		{
			if (this->_key == true && this->_password == key)
			{
				this->_key = false;
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -k " + key + "\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -k " + key + "\r\n";
				client->send_msg(Msg);
			}
			else if (this->_key == false)
			{
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -k\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -k" + "\r\n";
				client->send_msg(Msg);
			}
		}
	}
	else
		server->sendError(client->getFd(), "482", this->_name,"You're not channel operator");
}

void Channel::changeModeO(Server *server, Client *client, std::string arg,
	Client *cible)
{
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible) == false)
		{
			server->sendError2(client->getFd(), "441", cible->getNick(),
				this->_name, "They aren't on that channel");
		}
		else
		{
			if (arg == "+o" && verifClientisUser(cible) == true)
			{
				userToOperator(cible);
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +o " + cible->getNick() +"\r\n";
				client->send_msg(Msg);
			}
			else if (arg == "+o" && verifClientisUser(cible) == false)
			{
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +o " + cible->getNick() +"\r\n";
				client->send_msg(Msg);
			}
			else if (arg == "-o" && verifClientisOperator(cible) == true)
			{
				operatorToUser(cible);
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, client);  
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -o " + cible->getNick() +"\r\n";
				client->send_msg(Msg);
			}
			else if (arg == "-o" && verifClientisOperator(cible) == false)
			{
				std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, client); 
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -o " + cible->getNick() +"\r\n";
				client->send_msg(Msg);
			}
		}
	}
	else
		server->sendError(client->getFd(), "482", this->_name,"You're not channel operator");
}

void Channel::changeModeL(Server *server, Client *client, std::string arg,
	int limit)
{
	if (verifClientisOperator(client) == true)
	{
		if (arg == "+l" && _hasLimit == false)
		{
			_hasLimit = true;
			_limit = limit;
			std::ostringstream oss;
			oss << limit;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +l " + oss.str() + "\r\n";			
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +l " + oss.str() +"\r\n";
			client->send_msg(Msg);
		}
		else if (arg == "+l" && _hasLimit == true)
		{
			_limit = limit;
			std::ostringstream oss;
			oss << limit;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " +l " + oss.str() + "\r\n";	
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +l " + oss.str() +"\r\n";
			client->send_msg(Msg);
		}
		else if (arg == "-l" && _hasLimit == true)
		{
			_hasLimit = false;
			std::string modeMsg = ":" + client->getPrefix() + " MODE " + _name + " -l " + "\r\n";	
			ChannelSend(modeMsg, client); 
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -l " +"\r\n";
			client->send_msg(Msg);
		}
	}
	else
		server->sendError(client->getFd(), "482", this->_name,"You're not channel operator");
}
