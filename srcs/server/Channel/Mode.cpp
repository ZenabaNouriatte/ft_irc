/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smolines <smolines@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:10:02 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/07 15:13:09 by smolines         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sstream>
#include <Channel.hpp>



// MODE #channel i
// i = on invitation only
// +i : seuls ls invites accedent
//-i : tout le monde accede
void Channel::changeModeI(Server *server, Client *client, std::string arg)
{
	if (verifClientisOperator(client) == true)
	{
		if (arg == "-i")
		{
			this->_inviteOnly = false;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -i\r\n";
			ChannelSend(modeMsg, NULL); 
			// reponse :<Nick>!user@host MODE #channel -i
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -i" + "\r\n";
			client->send_msg(Msg);
			// message :serveur 324 <nick> #channel -i
		}
		else if (arg == "+i")
		{
			this->_inviteOnly = true;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +i\r\n";
			ChannelSend(modeMsg, NULL); 
			// reponse :<Nick>!user@host MODE #channel +i
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +i" + "\r\n";
			client->send_msg(Msg);
			// message :serveur 324 <nick> #channel +i
		}
	}
	else
	{
		std::cout << "DEBUG ChangeModeI : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
	}
}

void Channel::changeModeT(Server *server, Client *client, std::string arg)
{
	if (verifClientisOperator(client) == true)
	{
		/// A SUPPRIMER /////
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
		/////
		if (arg == "+t")
		{
			this->_topicRestriction = true;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +t\r\n";
			ChannelSend(modeMsg, NULL); 
			// message: :<nick>!user@host MODE #channel +t
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +t" + "\r\n";
			client->send_msg(Msg);
			// message :serveur 324 <nick> #channel +t
		}
		if (arg == "-t")
		{
			this->_topicRestriction = false;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -t\r\n";
			ChannelSend(modeMsg, NULL); 
			// message: :<nick>!user@host MODE #channel -t
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -t" + "\r\n";
			client->send_msg(Msg);
			// message :serveur 324 <nick> #channel -t
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
			{
				server->sendError(client->getFd(), "467", this->_name,
					"Channel key already set");
				std::cout << "DEBUG ChangeModeK : channel a deja un password" << std::endl;
			}
			else if (this->_key == false && isValidChannelPW(key) == true)
			{
				this->_password = key;
				this->_key = true;
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +k\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!user@host MODE #channel +k
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +k " + key + "\r\n";
				client->send_msg(Msg);
				// :serveur 324 <nick> #channel +k motdepasse
			}
			else
			{
				std::cout << "DEBUG ChangeModeK : bad channel key - password non valid" << std::endl;
				server->sendError(client->getFd(), "475", this->_name,
					"Cannot join channel (+k)");
			}
		}
		if (arg == "-k")
		{
			if (this->_key == true && this->_password == key)
			{
				this->_key = false;
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -k\r\n";
				ChannelSend(modeMsg, NULL); 

				// message: :<nick>!user@host MODE #channel -k
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -k" + "\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #channel -k
			}
			else if (this->_key == false)
			{
				std::cout << "DEBUG ChangeModeK : channel est deja en mode -k : message confirmation" << std::endl;
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -k\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!user@host MODE #channel -k
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -k" + "\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #channel -k
			}
			else
			{
				std::cout << "DEBUG ChangeModeK : mauvais password de channel : mot de passe ignore et mode -k active" << std::endl;
				// attention infos contradictoires trouvees sur le sujet
				// server->sendError(client->getFd(), "475", this->_name, "Cannot join channel (+k)");
				// message: :<nick>!user@host MODE #channel -k
				//:serveur 324 <nick> #channel -k
			}
		}
	}
	else
	{
		std::cout << "DEBUT ChangeModeK : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
	}
}

void Channel::changeModeO(Server *server, Client *client, std::string arg,
	Client *cible)
{
	if (verifClientisOperator(client) == true)
	{
		if (verifClientisInChannel(cible) == false)
		{
			std::cout << "DEBUT ChangeModeO : Cible not in channel" << std::endl;
			server->sendError2(client->getFd(), "441", cible->getNick(),
				this->_name, "They aren't on that channel");
		}
		else
		{
			if (arg == "+o" && verifClientisUser(cible) == true)
			{
				std::cout << "DEBUG ChangeModeO : cible passe de user a operator" << std::endl;
				userToOperator(cible);
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -o" + cible->getNick() +"\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #canal +o cible
			}
			else if (arg == "+o" && verifClientisUser(cible) == false)
			{
				std::cout << "DEBUT ChangeModeO : cible already operator : envoi message confirmation" << std::endl;
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +o" + cible->getNick() +"\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #canal +o cible
			}
			else if (arg == "-o" && verifClientisOperator(cible) == true)
			{
				std::cout << "DEBUG ChangeModeO : cible passe de operator a user" << std::endl;
				operatorToUser(cible);
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!<user>@<host> MODE #canal -o cible
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -o" + cible->getNick() +"\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #canal -o cible
			}
			else
			{
				std::cout << "DEBUT ChangeModeO : cible not an operator : envoi message confirmation" << std::endl;
				std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +o " + cible->getNick() + "\r\n";
				ChannelSend(modeMsg, NULL); 
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -o" + cible->getNick() +"\r\n";
				client->send_msg(Msg);
				//:serveur 324 <nick> #canal +o cible
			}
		}
	}
	else
	{
		std::cout << "DEBUT ChangeModeO : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
	}
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
			std::cout << "DEBUT ChangeModeO : cible already operator : envoi message confirmation" << std::endl;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +l " + oss.str() + "\r\n";			
			// message confirmation : :<nick>!<user>@<host> MODE #canal +l 10
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +l" + oss.str() +"\r\n";
			client->send_msg(Msg);
			// :serveur 324 <nick> #canal +l 10
		}
		else if (arg == "+l" && _hasLimit == true)
		{
			_limit = limit;
			std::ostringstream oss;
			oss << limit;
			std::cout << "DEBUT ChangeModeL : limite modifiee" << std::endl;
			std::cout << "DEBUT ChangeModeO : cible already operator : envoi message confirmation" << std::endl;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " +l " + oss.str() + "\r\n";	
			// message confirmation : :<nick>!<user>@<host> MODE #canal +l 10
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " +l" + oss.str() +"\r\n";
			client->send_msg(Msg);
			//:serveur 324 <nick> #canal +l 10
		}
		else if (arg == "-l" && _hasLimit == true)
		{
			_hasLimit = false;
			std::cout << "DEBUT ChangeModeO : cible already operator : envoi message confirmation" << std::endl;
			std::string modeMsg = ":" + client->getPrefix() + " MODE :" + _name + " -l " + "\r\n";	
			// message confirmation : :<nick>!<user>@<host> MODE #canal -l
			std::string Msg = ":" + server->getServerName() + " 324 " + client->getNickname() + " " + _name + " -l" +"\r\n";
			client->send_msg(Msg);
			//:serveur 324 <nick> #canal -l
		}
		else
		{
			std::cout << "DEBUT ChangeModeL : no limit is set - commande ignoree" << std::endl;
			// commande ignoree - aucun message
		}
	}
	else
	{
		std::cout << "DEBUT ChangeModeL : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name,
			"You're not channel operator");
	}
}
