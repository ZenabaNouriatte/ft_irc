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
    private:
        int _fd;
        std::string _nickname;
        std::string _buffer; // Pour stocker les messages en attente

    public :
        Client(int fd);
        ~Client();
        void appendToBuffer(const std::string& data);
        std::string& getBuffer();

        int getFd(void) const;
        void send_msg(const std::string& message);
};
