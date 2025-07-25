/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:31:03 by cschmid           #+#    #+#             */
/*   Updated: 2025/07/25 10:12:42 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    // ?? must be an integer between 6660 and 6669 ??
    if (port < 1024 || port > 65535) 
    {
        std::cerr << "Error: Port number must be between 1024 and 65535." << std::endl;
        return (1);
    }
    std::string password = argv[2];
    Server server(port, password);
    signal(SIGINT, Server::catchSignal);
    server.start();
    return 0;
}
