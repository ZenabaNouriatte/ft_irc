/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cschmid <cschmid@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 20:18:15 by zmogne            #+#    #+#             */
/*   Updated: 2025/08/08 16:42:26 by cschmid          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::disconnectClient(int clientFd)
{
    std::map<int, Client*>::iterator it = _clients.find(clientFd);
    if (it == _clients.end()) 
        return;

    Client* client = it->second;
    if (client && client->isRegistered())
        client->setNickname("");
    _clients.erase(it);
    for (std::vector<struct pollfd>::iterator p = _poll_fds.begin(); p != _poll_fds.end(); ++p) 
    {
        if (p->fd == clientFd) 
        {
            _poll_fds.erase(p);
            break;
        }
    }
    if (close(clientFd) == -1) 
    {
        std::cerr << "ERROR: Failed to close fd " << clientFd << ": " << strerror(errno) << std::endl;
    } 
    
    if (client) 
        delete client;

    std::cout << RED << BOLD << "Client [" << clientFd << "] disconnected\n" << RESET << std::endl;
}

void Server::handleClientDisconnection(Client* client, ssize_t received_bytes)
{
    if (received_bytes == 0)
    {
        if (client->hasPartialData()) 
        {
            std::string incomplete_cmd = client->extractIncompleteCommand();
            if (!incomplete_cmd.empty()) 
            {
                Message msg(incomplete_cmd);
                handleCommand(client, msg);
            }
        }
    }
    Message quitMsg("QUIT :Client disconnected");
	handleQUIT(client, quitMsg);
}


void Server::cleanupChannels() 
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it) 
			delete *it;
	}
	_channels.clear();
}

void Server::cleanExit() 
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
        close(_poll_fds[i].fd);

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;

    _poll_fds.clear();
    _clients.clear();
    cleanupChannels();
}