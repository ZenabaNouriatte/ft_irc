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

bool Client::operator==(const Client& other) const {
			return this->getFd() == other.getFd(); 
		}


int Client::getFd(void) const
{
    return (_fd);
}