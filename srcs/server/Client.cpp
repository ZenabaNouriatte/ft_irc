#include "Client.hpp"

/*========== CONSTRUCTEUR ==========*/

Client::Client(int fd) : _fd(fd), _has_pass(false), _has_nick(false), _has_user(false), _registred(false) {}

/*========== DESTRUCTEUR ==========*/

Client::~Client() {
    close(_fd);
}

/*========== SETTER ==========*/

void Client::setHasPass(bool value){
    _has_pass = value;
}

void Client::setNickname(const std::string& nick){
    _nickname = nick; 
}

void Client::setHasNick(bool value) {
    _has_nick = value;
}

void Client::setUsername(const std::string& name){
    _username = name;
}

void Client::setRealname(const std::string& name){
    _realname = name;
}

void Client::setHasUser(bool value){
    _has_user = value;
}

void Client::setRegistered(bool value){
    _registred = value;
}

/*========== GETTER ==========*/

std::string Client::getNickname() const {
    return _nickname;
}

std::string& Client::getBuffer() 
{
    return _buffer;
}

int Client::getFd(void) const{
	return (_fd);
}

/*========== METHODES ==========*/
bool Client::isRegistered() const 
{
    (void)_registred;
    if (_has_pass && _has_nick && _has_user)
        return true;
    else
        return false;
}

void Client::appendToBuffer(const std::string& data) 
{
    _buffer += data;
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