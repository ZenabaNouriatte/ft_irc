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