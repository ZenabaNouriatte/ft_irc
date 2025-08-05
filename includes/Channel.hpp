#pragma once
#include <string>
#include <string.h>
#include <vector>
#include "Client.hpp"
#include <set>
#include "Server.hpp"

class Server;
class Client;
class Channel
{
	private :
		std::string _name;
		std::string _topicName; // theme du channel
		std::string _password;
		bool _inviteOnly; // channel sur invitation uniquement : 0 = non, 1 = oui
		bool _topicRestriction; // false = tous les users peuvent changer le topic / true ; que chanop
		bool _topic; // defini si un topic existe ou non
		size_t _limit; // nb max d'utilisateurs sur le channel
		bool _hasLimit; // defini si une limite max d'user existe
		bool _key; // defini si un password existe
		std::vector<Client*> _users; // liste des users du channel : peut etre repere par nickname ou par fd
		std::vector<Client*> _operators; // liste des operators : permet de changer ou ajouter operator
		std::vector<Client*> _isInvited; // liste des operators : permet de changer ou ajouter operator

	public :
		Channel();
		Channel(std::string name);
//		Channel(std::string name, std::string topicName);
		~Channel();
	

	//getters

	std::string getName();
	std::string getTopicName();
	std::string getPassWord();
	bool getTopicRestriction();
	bool getInviteOnly();
	bool getTopic();
	int getLimit();
	bool getHasLimit();
	bool getKey();
	std::vector<Client*> getUsers();
	std::vector<Client*> getOperators();

	//setters

	void setName(std::string name);
	void setTopicName(std::string topic);
	void setPassWord(std::string psswd);
	void setTopicRestriction(bool rectriction);
	void setInviteOnly(bool invite);
	void setTopic(bool topic);
	void setLimit(int nb);
	void setHasLimit(bool limit);
	void setKey(bool key);


	// fonctions

	void addUser(Server* server, Client* user, std::string key);
	void addOperator(Client* user);
	void addInvited(Client* client);
	void removeUser(int fd);
	void removeInvited(Client* user);
	void removeOperator(Client* user);
	void userToOperator (Client* user);
	void operatorToUser (Client* user);
	int affectNextOperator();

	int isChannelEmpty() const; // renvoie le nombre d'utilisateurs du channel
	bool isValidChannelPW(const std::string& password);
	bool verifClientisOperator (Client* client);
	bool verifClientisUser (Client* client);
	bool verifClientisInChannel (Client* client);
	bool verifClientisInvited(Client* client);

	
	// modes


	void changeModeI(Server* server, Client* client, std::string arg);
	void changeModeT(Server* server, Client* client, std::string arg);
	void changeModeK(Server* server, Client* client, std::string arg, std::string key);
	void changeModeO(Server* server, Client* client, std::string arg, Client* cible) ;
	void changeModeL(Server* server, Client* client, std::string arg, int limit);
	void changeTopic(Server* server, Client* client, std::string topic);

	//commandes op

	void commandTopic(Server* server, Client* client, std::string topic);
	void commandInvite(Server* server, Client* client, Client* cible);
	void commandKick(Server* server, Client* client, Client* cible, std::string comment);

	// ajout Zenaba debug
	void ChannelSend(const std::string& message, Client* sender);
	void printUsers() const;
	void printClientVectors() const;
	int getClientCount() const;

	// ajout Chloe
	void changeTopic(Client client, std::string topic);
	Client* findClientByNick(const std::string &nick);

};

