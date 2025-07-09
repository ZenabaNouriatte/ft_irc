#pragma once
#include <iostream>
#include <cstdlib> 
#include <sys/socket.h> // socket, bind, listen, accept
#include <netinet/in.h> // sockaddr_in, htons, INADDR_ANY
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <string>
#include <poll.h>

class Client 
{
    public:
        int fd;
        std::string nickname;
        std::string buffer; // Pour stocker les messages en attente

        Client(int fd) : fd(fd) {}
};
