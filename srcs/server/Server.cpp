#include "Server.hpp"

Server::Server(int port, const std::string& password) : _port(port), _password(password)
{
    std::cout << "Arg[1] = port : " << this->_port
                << "\nArg[2] = password : " << this->_password << std::endl;
}

Server::~Server() 
{
    std::cout << "Server destroyed\n";
}
