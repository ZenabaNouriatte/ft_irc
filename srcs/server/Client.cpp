#include "Client.hpp"

Client::Client(int fd) : _fd(fd) {}

Client::~Client() {
    close(_fd);
}

void Client::appendToBuffer(const std::string& data) 
{
    _buffer += data;
}

std::string& Client::getBuffer() 
{
    return _buffer;
}

int Client::getFd(void) const{
	return (_fd);
}

void Client::send_msg(const std::string& message)
{
    std::string full_message = message + "\r\n";

    ssize_t sent = send(_fd, full_message.c_str(), full_message.size(), 0);

    if (sent == -1)
    {
        std::cerr << "[ERROR] Echec de l'envoi du message au client." << _fd << std::endl;
    }
}
