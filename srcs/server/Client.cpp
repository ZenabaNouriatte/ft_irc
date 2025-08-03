/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zmogne <zmogne@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/21 14:30:25 by cschmid           #+#    #+#             */
/*   Updated: 2025/08/03 18:02:05 by zmogne           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/*========== CONSTRUCTEUR ==========*/

Client::Client(int fd) : _fd(fd), _has_pass(false), _has_nick(false), _has_user(false), _registred(false) {}

/*========== DESTRUCTEUR ==========*/

Client::~Client() {
    close(_fd);
}

/*========== GETTERS ==========*/

bool Client::hasPass() const { return _has_pass; }
bool Client::hasNick() const { return _has_nick; }
bool Client::hasUser() const { return _has_user; }

const std::string &Client::getPass() const { return _pass;}
const std::string &Client::getNick() const { return _nickname;}
const std::string &Client::getUser() const { return _username;}
const std::string &Client::getRealname() const { return _realname;}


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

void Client::setPass(const std::string& pass){
    _pass = pass;
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

std::string Client::getUsername() const {
    return _username;
}

std::string& Client::getBuffer() 
{
    return _buffer;
}

int Client::getFd(void) const
{
	return (_fd);
}

std::string Client::getPrefix() const 
{
    if (_nickname.empty() || _username.empty()) {
        std::ostringstream oss;
        oss << "user" << _fd << "@localhost";
        return oss.str();
    }
    return _nickname + "!" + _username + "@localhost";
}


/*========== METHODES ==========*/
bool Client::isRegistered() const 
{
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
        std::cerr << RED << BOLD << "[ERROR] Error sending msg to client." << _fd << RESET << std::endl;
    }

}

std::vector<std::string> Client::extractCompleteCommands() 
{
    std::vector<std::string> commands;
    std::string::size_type pos = 0;

    while ((pos = _buffer.find('\n')) != std::string::npos) 
    {
        std::string command = _buffer.substr(0, pos);
        _buffer.erase(0, pos + 1); // supprimer la ligne + '\n'

        if (!command.empty() && command[command.length() - 1] == '\r') 
            command.erase(command.length() - 1); // enlever le '\r'

        if (!command.empty()) 
            commands.push_back(command);
    }

    return commands;
}

//forcer l'extraction d'une commande incomplete ( Ctrl+D)
std::string Client::extractIncompleteCommand() 
{
    std::string command = _buffer;
    _buffer.clear();
    return command;
}

bool Client::hasPartialData() const 
{
    return !_buffer.empty();
}

void Client::clearBuffer() 
{
    _buffer.clear();
}

bool Client::operator==(const Client& other) const {
			return this->getFd() == other.getFd(); 
		}


