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
#include <map>
#include <Channel.hpp>
#include <sstream>


# define RESET "\033[0m"
# define BOLD "\033[1m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define CYAN "\033[36m"

class Client 
{
    private:
        int _fd;
        std::string _nickname;
        std::string _username;
        std::string _realname;
        std::string _pass;
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
        void setHasPass(bool value);
        void send_msg(const std::string& message);
        void setNickname(const std::string& nick);
        std::string getNickname() const;
        std::string getUsername() const;
        void setHasNick(bool value);
        void setUsername(const std::string& name);
        void setRealname(const std::string& name);
        void setHasUser(bool value);
        void setRegistered(bool value);
        void setPass(const std::string &pass);
        std::string getPrefix() const;

        bool hasPass() const;
        bool hasNick() const;
        bool hasUser() const;

        const std::string &getPass() const;
        const std::string &getNick() const;
        const std::string &getUser() const;
        const std::string &getRealname() const;

        std::vector<std::string> extractCompleteCommands();
        std::string extractIncompleteCommand();
        bool hasPartialData() const;
        void clearBuffer();

		bool operator==(const Client& other) const;
		
};
