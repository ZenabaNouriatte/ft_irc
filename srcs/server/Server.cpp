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

void Server::handleError(const std::string& message) 
{
    std::cerr << message << std::endl;
    if (_server_fd >= 0)
        close(_server_fd);
}

void Server::start()
{
    _server_fd = socket(AF_INET, SOCK_STREAM,0); // creer un socket : famille d'adresse ipv4 , type TCP , protocole par default (tcp)
    if (_server_fd == -1)
        return handleError ("Fail socket\n");

    sockaddr_in addr; // structure pour ipv4
    addr.sin_family = AF_INET; // type d'adresse
    addr.sin_port = htons(_port); //port choisi converti au format reseau
    addr.sin_addr.s_addr = INADDR_ANY; // ecoute toutes les interface 
    // écouter sur ce port sur toutes les IPs de cette machine
    socklen_t f_addr_len = sizeof(addr);
    if (bind(_server_fd, (sockaddr*)&addr, f_addr_len) == -1) // spécifier le type de communication associé au socket (protocole TCP ou UDP) 
        return handleError ("Fail bind\n");
    //bind() lie le socket _server_fd a l’adresse configurer
    if (listen(_server_fd, 5)  == -1) //placer le socket en mode passif (à l'écoute des messages).
        return handleError ("Fail listen\n");
    //Attend des connexions
    //5 = nombre max de client en attente 
    std::cout << "Server ready on port: " << _port << std::endl;

    socklen_t addr_len = sizeof(addr);
    std::cout << "Waiting for a client..." << std::endl;
    _socket_fd = accept(_server_fd, (sockaddr*)&addr, &addr_len);
    if (_socket_fd == -1)
        return handleError("Fail accept");
    std::cout << "Client connected !" << std::endl;

    std::string buffer (BUFFER_SIZE, '\0');
    std::string input;
    // si utilisation de poll()
    while (1)
    {
        int receivedBytes = recv(_socket_fd, &buffer[0], BUFFER_SIZE, 0);
        if (receivedBytes == 0) 
        {
            std::cout << "Client disconnected." << std::endl;
            break;
        }
        if (receivedBytes == -1)
            return handleError("Serveur - erreur reception du msg du clint");
        
        buffer.resize(receivedBytes);
        buffer.erase(std::remove(buffer.begin(), buffer.end(), '\r'), buffer.end());
        buffer.erase(std::remove(buffer.begin(), buffer.end(), '\n'), buffer.end());
        std::cout << "Client > " << buffer << std::endl;
        if (std::string(buffer) == "exit")
            break;
        std::cout << "Serveur > ";
        std::getline(std::cin, input);
        int sentBytes = send (_socket_fd, input.c_str(), input.length(), 0);
        if (sentBytes == -1)
            handleError("Serveur - fail envoi msg");
        input += "\n"; 
        if (std::string(buffer) == "exit")
            break;        
    }
    close (_socket_fd);
    close (_server_fd);
}


