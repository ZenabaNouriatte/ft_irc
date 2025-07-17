#include <Channel.hpp>

Channel::Channel(std::string name)
{
	this->_name = name;
	this->_inviteOnly = 0;
	this->_limit = 0;
	this->_topic = false;
	this->_topicRestriction = false; 
	this->_hasLimit = false; 
	this->_key = false; 
	return;
}

Channel::Channel(std::string name, std::string topicName)
{
	this->_topicName = topicName;
	this->_name = name;
	this->_inviteOnly = 0;
	this->_limit = 0;
	this->_topic = true;
	this->_topicRestriction = false; 
	this->_hasLimit = false; 
	this->_key = false; 

	return;
}


Channel::~Channel()
{
	return;
}

////// ACCESSSORS //////


	//getters
	bool Channel::getTopic() { return this->_topic; }
	std::string Channel::getName() { return this->_name; }
	std::string Channel::getTopicName() { return _topicName; }
	std::string Channel::getPassWord() { return this->_password; }
	bool Channel::getTopicRestriction() { return this->_topicRestriction; }
	bool Channel::getInviteOnly() { return this->_inviteOnly; }
	int Channel::getLimit() { return this->_limit; }
	bool Channel::getHasLimit() { return this->_hasLimit; }
	bool Channel::getKey() { return this->_key; }
	std::vector<Client> Channel::getUsers()  { return this->_users; }
	std::vector<Client> Channel::getOperators() { return this->_operators; }

	//setters
	void Channel::setName(std::string name){
		this->_name = name;
		return;	}

	void Channel::setTopicName(std::string topic) {
		this->_topicName = topic;
		setTopic(true);
		return;	}

	void Channel::setPassWord(std::string psswd){
		this->_password =  psswd;
		setKey(true);
		return; }

	void Channel::setTopicRestriction(bool rectriction){
		this->_topicRestriction = rectriction;
		return;	}

	void Channel::setInviteOnly(bool invite){
		this->_inviteOnly = invite;
		return;	}

	void Channel::setTopic(bool topic){
		this->_topic = topic;
		return;	}

	void Channel::setLimit(int nb){
		this->_limit = nb;
		setHasLimit(true);
		return;	}

	void Channel::setHasLimit(bool limit){
		this->_hasLimit = limit;
		return;	}

	void Channel::setKey(bool key){
		this->_key = key;
		return;	}



	int Channel::isChannelEmpty()  // renvoie le nombre d'utilisateurs connectes au channel : user + operator
	{
		return (_users.size() + _operators.size()); 
	}


	void Channel::addUser(const Client &user) {
		if (_operators.empty())
			_operators.push_back(user);
		else if (_hasLimit == true && (_users.size() + _operators.size()) < _limit) {
			std::cout << "Too much users in this channel" << std::endl;
			//// refuser acces ////
		}
		else
			_users.push_back(user);
		return;
	}

	void Channel::addOperator(const Client &user)   // a la creation : operator = user numero 1. Si user#1 quitte : definir un nouvel operator // ajouter un op
	{
		 if (std::find(_users.begin(), _users.end(), user.getName()) != _users.end()) 
			userToOperator(user);
		else
			_operators.push_back(user);
	}

	void removeUser(int fd) {
    for (std::vector<Client>::iterator it = _users.begin(); it != _users.end(); ++it) {
        if (it->getFd() == fd) 
            _users.erase(it);
            return;
		}

	void removeOperator(const Client &user){
		_operators.erase(user);	
	}
	void userToOperator (const Client &user) {
		_operators.push_back(user);
		_users.erase(user);
	}
	void operatorToUser (const Client &user) {
		_users.push_back(user);
		_operator.erase(user);
	}




// arrivee de la commande :
// JOIN <channel>{,<channel>} [<key>{,<key>}]


// commandes operateur :

 KICK - Ejecter un client du channel
 INVITE - Inviter un client au channel
 TOPIC - Modifier ou afficher le thème du channel
 MODE - Changer le mode du channel :
— i : Définir/supprimer le canal sur invitation uniquement
— t : Définir/supprimer les restrictions de la commande TOPIC pour les opérateurs de canaux
— k : Définir/supprimer la clé du canal (mot de passe)
— o : Donner/retirer le privilège de l`opérateur de canal
— l : Définir/supprimer la limite d`utilisateurs pour le canal


