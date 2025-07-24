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



	int Channel::isChannelEmpty() const // renvoie le nombre d'utilisateurs connectes au channel : user + operator
	{
		return (_users.size() + _operators.size()); 
	}


	void Channel::addUser(const Client &user) {
		// rajouter ici condition du not de passe : if (_key == true)		

		if (_operators.empty())
			_operators.push_back(user);
			//message

		else if (_hasLimit == true && (_users.size() + _operators.size()) < _limit) {
			std::cout << "Too much users in this channel" << std::endl;
			//// refuser acces ////
			///message///
			}	
		// if (_inviteOnly == true)
		// 	// utilisateur sur invitation
	
		// else
		// 	_users.push_back(user);
		// 	//message
		return;
	}

	void Channel::addOperator(const Client &client)   
	// a la creation : operator = user numero 1. Si user#1 quitte : definir un nouvel operator // ajouter un op
	{
		if (verifClientisOperator (client) == true )
				userToOperator(client);
		else
			_operators.push_back(client);
	}

	void Channel::removeUser(int fd) {
    	for (std::vector<Client>::iterator it = _users.begin(); it != _users.end(); ++it) 
		{
    	    if (it->getFd() == fd) 
    	        _users.erase(it);
		}
	    return;
	}

	void Channel::removeOperator(const Client &user){
		std::vector<Client>::iterator it = std::find(_operators.begin(), _operators.end(), user);
		if (it != _operators.end())
    		_operators.erase(it);
	}

	void Channel::userToOperator (const Client &user) {
		_operators.push_back(user);
		std::vector<Client>::iterator it = std::find(_users.begin(), _users.end(), user);
		if (it != _users.end())
    		_users.erase(it);
	}

	void Channel::operatorToUser (const Client &user) {
		_users.push_back(user);
		std::vector<Client>::iterator it = std::find(_operators.begin(), _operators.end(), user);
		if (it != _operators.end())
    		_operators.erase(it);
	}


	bool Channel::verifClientisInChannel (Client & client) {
		if (std::find(_operators.begin(), _operators.end(), client.getNick()) != _operators.end() 
			&& std::find(_users.begin(), _users.end(), client.getNick()) != _users.end())
				return true;
		return false;
	}


	bool Channel::verifClientisOperator (Client & client) {
		if (std::find(_operators.begin(), _operators.end(), client.getNick()) != _operators.end()) 
			return true;
		return false;
	}

	bool Channel::verifClientisUser (Client & client) {
		if (std::find(_users.begin(), _users.end(), client.getNick()) != _users.end()) 
			return true;
		return false;
	}


	bool isValidChannelPW(const std::string& password) {
    	if (password.empty() || password.size() > 23)
    	    return false;
		for (size_t i = 0; i < password.size(); ++i) 
		{
        	char c = password[i];
        if (std::isspace(static_cast<unsigned char>(c)) || c == ':' || c == ',')
            return false;
    	}
		return true;
	}


	// MODE #channel i
// i = on invitation only
// +i : seuls ls invites accedent
//-i : tout le monde accede
	void Channel::changeModeI(Client & client, std::string arg) {
		if (verifClientisOperator (client) == true )
		{
			if (arg == "-i" && _inviteOnly == true)
				this->_inviteOnly = false;	
				// message confirmation
				// reponse a envoyer : <server> MODE #chan -i

			else if (arg == "+i" && _inviteOnly == false)
				this->_inviteOnly = true;	
				//message confirmation
			//else
				// le mode demande est deja en cours
		}
		//else
		//error
		// message pas de droit pour changer le mode
		}

	void Channel::changeModeT(Client client, std::string arg) {
		if (verifClientisOperator (client) == true )
		{
			if (arg == "+t") 
				this->_topicRestriction = true;
				//message
			if (arg == "-t")
				this->_topicRestriction = false;
				//message
		}
	}



	void Channel::changeTopic(Client client, std::string topic) {
		if (_topicRestriction == true && verifClientisOperator(client) == false)
		{
			std::cout << "DEBUG changeTopic : refus de changement" << std::endl;
			// refus de changement
			// message
		}
		else
		{
			this->_topicName = topic;
			this->_topic = true;
			//message
		}
	}

	void Channel::changeModeK(Client client, std::string arg, std::string key) 
	{
			if (verifClientisOperator (client) == true )
			{
				if (arg == "+k")
				{
					if (this->_key == true)
						// 467 <nick> #canal :Channel key already set
						this->_password = key;
					else if (this->_key == false && isValidChannelPW(key) == true)
						this->_password = key;
					//else
						//message mauvais mot de passe : ERR_BADCHANNELKEY
				}
				if (arg == "-k")
				{	
					if (this->_key == true && this->_password == key)
							this->_key = false;
						//message desactivation key
					//else if (this->_key == false)	
						//message mode key non active
					//else 
						//message mauvais mot de passe : ERR_BADCHANNELKEY
				}
			}
			else
				//erreur client not operator

	}

void Channel::changeModeO(Client client, std::string arg, Client cible) 
{
	if (verifClientisOperator(client) == true )
	{
		if (verifClientisInChannel == false)
			// message client not in channel
		else		
		{
			if (arg == "+o" && verifClientisUser(cible) == true)
				// remove cible from users
				// add cible to operator
			else if (arg == "+o" && verifClientisUser(cible) == false)
				// message cible already operator

			else if (arg == "-o" && verifClientisOperator(cible) == true)
			// remove cible from operator
			// add cible to user

			else
			// // message cible not an operator
		}
	}
	else
		//erreur client not operator
}


void Channel::changeModeL(Client client, std::string arg, int limit) 
{
	if (verifClientisOperator(client) == true )
	{
		if (arg == "+l" && _hasLimit == false)
			{
			_hasLimit == true;
			_limit = limit;
			}
		else if (arg == "+l" && _hasLimit == true)
			//message error limit already set
		else if (arg == "-l" && _hasLimit == true)
			_hasLimit == false;
		else 
			//message error no limit is set
	}
		else 
			//erreur client not operator
}

