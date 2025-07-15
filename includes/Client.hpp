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
        std::string _username;
        std::string _realname;
        bool _has_pass;
        bool _has_nick;
        bool _has_user;
        bool _registred;
        std::string _buffer; // Pour stocker les messages en attente

    public :
        Client(int fd);
        ~Client();
        bool isRegistered() const ;
        void appendToBuffer(const std::string& data);
        std::string& getBuffer();

        int getFd(void) const;
        void send_msg(const std::string& message);
};
