#pragma once
#include <string>
#include <string.h>
#include <vector>
#include "Client.hpp"

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
		std::vector<Client> _users; // liste des users du channel : peut etre repere par nickname ou par fd
		std::vector<Client> _operators; // liste des operators : permet de changer ou ajouter operator
	

	public :
		Channel();
		Channel(std::string name);
		Channel(std::string name, std::string topicName);
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
	std::vector<Client> getUsers();
	std::vector<Client> getOperators();

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

    void addUser(const Client &user);

};
