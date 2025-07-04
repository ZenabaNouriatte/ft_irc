#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password), _server_fd(-1)
{
    std::cout << "Arg[1] = port : " << this->_port
                << "\nArg[2] = password : " << this->_password << std::endl;
}

Server::~Server() 
{
    std::cout << "Server destroyed\n";
}

void Server::start()
{
    _server_fd = socket(AF_INET, SOCK_STREAM,0); // creer un socket : famille d'adresse ipv4 , type TCP , protocole par default (tcp)
    if (_server_fd < 0)
    {
        std::cerr << "Fail socket\n";
        return;
    }
    sockaddr_in addr; // structure pour ipv4
    addr.sin_family = AF_INET; // type d'adresse
    addr.sin_port = htons(_port); //port choisi converti au format reseau
    addr.sin_addr.s_addr = INADDR_ANY; // ecoute toutes les interface 
    // écouter sur ce port sur toutes les IPs de cette machine
    if (bind(_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        std::cerr << "Fail bind\n";
        return;
    }
    //bind() lie le socket _server_fd a l’adresse configurer
    if (listen(_server_fd, 5) < 0)
    {
        std::cerr << "Fail listen\n";
        return;
    }
    //Attend des connexions
    //5 = nombre max de client en attente 
    std::cout << "Server ready on port: " << _port << std::endl;

}