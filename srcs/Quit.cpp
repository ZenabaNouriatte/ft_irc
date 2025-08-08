/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 20:18:15 by zmogne            #+#    #+#             */
/*   Updated: 2025/08/07 23:33:54 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::disconnectClient(int clientFd)
{
    std::cout << "DEBUG: Attempting to disconnect client " << clientFd << std::endl;
    std::map<int, Client*>::iterator it = _clients.find(clientFd);
    if (it == _clients.end()) 
    {
        std::cout << " DEBUG WARNING: Client " << clientFd << " not found in _clients map" << std::endl;
        return;
    }

    Client* client = it->second;
    if (client && client->isRegistered())
    {
        client->setNickname("");
        std::cout << "DEBUG: Removed nickname from nickname registry" << std::endl;
    }
    _clients.erase(it);
    for (std::vector<struct pollfd>::iterator p = _poll_fds.begin(); p != _poll_fds.end(); ++p) 
    {
        if (p->fd == clientFd) 
        {
            _poll_fds.erase(p);
            std::cout << "DEBUG: Removed fd " << clientFd << " from _pollfds" << std::endl;
            break;
        }
    }


    if (close(clientFd) == -1) 
    {
        std::cerr << "ERROR: Failed to close fd " << clientFd << ": " << strerror(errno) << std::endl;
    } 
    else // DEBUG
    {
        std::cout << "DEBUG: Closed socket fd " << clientFd << std::endl;
    }
    if (client) 
    {
        delete client;
        std::cout << "DEBUG: Deleted Client* for fd " << clientFd << std::endl;
    }

    std::cout << RED << BOLD << "Client [" << clientFd << "] disconnected\n" << RESET << std::endl;
}

void Server::handleClientDisconnection(Client* client, int client_fd, ssize_t received_bytes)
{
    if (received_bytes == 0)
    {
        std::cout << RED << BOLD << " DEBUG Client [" << client_fd << "] closed the connection." << RESET << std::endl;

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
    else
        handleError("DEBUG ERROR recv()");
    Message quitMsg("QUIT :Client disconnected");
	handleQUIT(client, quitMsg);
}


void Server::cleanupChannels() 
{
	std::cout << "DEBUG: Cleaning up channels..." << std::endl;
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		if (*it) 
        {
			std::cout << "DEBUG: Deleting channel " << (*it)->getName() << std::endl;
			delete *it;
		}
	}
	_channels.clear();
	std::cout << "DEBUG: Channel cleanup completed." << std::endl;
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