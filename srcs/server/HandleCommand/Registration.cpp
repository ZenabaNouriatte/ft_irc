/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Registration.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:02:04 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/31 14:02:17 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::welcomeServer()
{
	std::cout << "\n██████████████████████████████████████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "███  ***  GOSSIP.IRC SERVER  ***  ████" << std::endl;
	std::cout << "███                               ████" << std::endl;
	std::cout << "██████████████████████████████████████" << std::endl;
	std::cout << "\n────────────────────────────\n" << std::endl;
	std::cout << "         SERVER LOG        :\n" << std::endl;
}

void Server::welcomeClient(Client *client)
{
	std::string nick = client->getNickname();
	client->send_msg(":" + _server_name + " 001 " + nick +
						" :\x03"
						"03"
						"Welcome to the GOSSIP.IRC Network, " +
						nick + "\x0F");
	client->send_msg(":" + _server_name + " 002 " + nick +
						" :\x03"
						"03"
						"Your host is " +
						_server_name + ", running version 1.0\x0F");
	client->send_msg(":" + _server_name + " 003 " + nick +
						" :\x03"
						"03"
						"This server was created July 2025\x0F");
	client->send_msg(":" + _server_name + " 004 " + nick +
						" :\x03"
						"03" +
						_server_name + " 1.0 o o\x0F");
}


void Server::completeRegistration(Client *client)
{
	client->setRegistered(true);
	this->welcomeServer();
	this->welcomeClient(client);
}