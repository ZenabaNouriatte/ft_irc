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
    std::cout << GREEN << BOLD << "Channel [" << this->_name << "] successfully created" << RESET << std::endl;
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

    std::cout << GREEN << BOLD << "Channel [" << this->_name << "] successfully created" << RESET << std::endl;
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
	std::vector<Client*> Channel::getUsers()  { return this->_users; }
	std::vector<Client*> Channel::getOperators() { return this->_operators; }
	
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

	void Channel::setTopic(bool topic)
    {
		this->_topic = topic;
		return;	}

	void Channel::setLimit(int nb)
    {
		this->_limit = nb;
		setHasLimit(true);
		return;	}

	void Channel::setHasLimit(bool limit)
    {
		this->_hasLimit = limit;
		return;	}

	void Channel::setKey(bool key)
    {
		this->_key = key;
		return;	}

	int Channel::isChannelEmpty() const // renvoie le nombre d'utilisateurs connectes au channel : user + operator
	{
		return (_users.size() + _operators.size()); 
	}

	void Channel::addOperator(Client* client)   
	// a la creation : operator = user numero 1. Si user#1 quitte : definir un nouvel operator // ajouter un op
	{
		if (verifClientisUser(client) == true)
			userToOperator(client);
		else if (verifClientisOperator(client) == false)
			_operators.push_back(client);
	}


	void Channel::addInvited(Client* client)   
	// ajouter un nick a la liste des invites
	{
		if (verifClientisInvited(client) == false)
			_isInvited.push_back(client);
	}


	void Channel::removeUser(int fd) 
	{
    	for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); ++it) 
		{
    	    if ((*it)->getFd() == fd) 
			{
				_users.erase(it);
				break;
			}
		}
	    return;
	}

	void Channel::removeOperator(Client* user){
		std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), user);
		if (it != _operators.end())
			_operators.erase(it);
	}

	void Channel::removeInvited(Client* user){
		std::vector<Client*>::iterator it = std::find(_isInvited.begin(), _isInvited.end(), user);
		if (it != _isInvited.end())
			_isInvited.erase(it);
	}
	
	void Channel::userToOperator (Client *user) {
		_operators.push_back(user);
		std::vector<Client*>::iterator it = std::find(_users.begin(), _users.end(), user);
		if (it != _users.end())
			_users.erase(it);
	}

	void Channel::operatorToUser (Client* user) {
		_users.push_back(user);
		std::vector<Client*>::iterator it = std::find(_operators.begin(), _operators.end(), user);
		if (it != _operators.end())
			_operators.erase(it);
	}

	bool Channel::verifClientisInChannel (Client* client) {
		if (std::find(_operators.begin(), _operators.end(), client) != _operators.end() 
			&& std::find(_users.begin(), _users.end(), client) != _users.end())
				return true;
		return false;
	}

	bool Channel::verifClientisOperator (Client* client) {
		if (std::find(_users.begin(), _users.end(), client) != _users.end()) 
			return true;
	}
	return false;


	bool Channel::verifClientisUser(Client* client) {
		return std::find(_users.begin(), _users.end(), client) != _users.end();
	}
	return false;



	bool Channel::verifClientisInvited(Client* client) {
		return std::find(_isInvited.begin(), _isInvited.end(), client) != _isInvited.end();
	}

	bool Channel::isValidChannelPW(const std::string& password) {
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


	void Channel::addUser(Server* server, Client* user, std::string key) {

		// si l'utilisateur est deja dans le channel (user ou chanop) :
		if (verifClientisInChannel (user) == true) 
		{
			server->sendError2(user->getFd(), "443", user->getNick(), this->_name, "is already on channel");
			std::cout << "DEBUG ADDUSER user already in the channel" << std::endl;
		}
		// si l'utilisateur est deja connecte a 10 channel /
		//DEBUG(client->getFd(), "405", "channel name", "You have joined too many channels");
		else if (_inviteOnly == true && verifClientisInvited(user) == false)
		{
			std::cout << "DEBUG ADDUSER channel on invite mode only" << std::endl;
			server->sendError(user->getFd(), "473", this->_name, "Cannot join channel (+i)");
		}
		else if (_hasLimit == true && (_users.size() + _operators.size()) < _limit) 
		{
			std::cout << "DEBUG ADDUSER Too much users in this channel" << std::endl;
			server->sendError(user->getFd(), "471", this->_name, "Cannot join channel (+l)");
		}	
		else if (this->_key && this->_password != key)
		{	
			server->sendError(user->getFd(), "475", this->_name, "Cannot join channel (+k)");
			std::cout << "DEBUG ADDUSER bad password" << std::endl;
		}
		else if (_operators.empty())
			_operators.push_back(user);
			//envoi des messages lies a la creation du channel ???
			//message: :<nick>!<user>@<host> JOIN :#channel
			//:<serveur> MODE #channel +o <nick>
			// :<serveur> 324 <nick> #channel +o
			// :<serveur> 353 <nick> = #channel :<nick>
			// :<serveur> 366 <nick> #channel :End of NAMES list
			// :<serveur> 331 <nick> #channel42 :No topic is set
		else 
			_users.push_back(user);
			//message: :<nick>!<user>@<host> JOIN :#channel
			// message : liste des modes du channel
			//message : liste des clients du channel
			//message : topic du channel
		return;
	}


// MODE #channel i
// i = on invitation only
// +i : seuls ls invites accedent
//-i : tout le monde accede
	void Channel::changeModeI(Server* server, Client* client, std::string arg) {
		if (verifClientisOperator (client) == true )
		{
			if (arg == "-i")
			{
				this->_inviteOnly = false;	
				// reponse :<Nick>!user@host MODE #channel -i
				// message :serveur 324 <nick> #channel -i
			}
			else if (arg == "+i")
			{
				this->_inviteOnly = true;	
				// reponse :<Nick>!user@host MODE #channel +i
				// message :serveur 324 <nick> #channel +i
			}
		}
		else
		{
			std::cout << "DEBUG ChangeModeI : client not an operator" << std::endl;
			server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
		}

		}

	void Channel::changeModeT(Server* server, Client* client, std::string arg) {
		if (verifClientisOperator (client) == true )
		{
			/// A SUPPRIMER /////
			server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
			/////
			if (arg == "+t") 
			{
				this->_topicRestriction = true;
				//message: :<nick>!user@host MODE #channel +t
				// message :serveur 324 <nick> #channel +t
			}
			if (arg == "-t")
			{
				this->_topicRestriction = false;
				//message: :<nick>!user@host MODE #channel -t
				// message :serveur 324 <nick> #channel -t
			}
		}
	}

	void Channel::changeTopic(Server* server, Client* client, std::string topic) {
		if (_topicRestriction == true && verifClientisOperator(client) == false)
		{
			std::cout << "DEBUG changeTopic : refus de changement" << std::endl;
			server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
		}
		else
		{
			this->_topicName = topic;
			this->_topic = true;
			//message: :<nick>!user@host MODE #channel +t
			// message :serveur 324 <nick> #channel -t
		}
	}

	void Channel::changeModeK(Server* server, Client* client, std::string arg, std::string key) 
	{
			if (verifClientisOperator (client) == true )
			{
				if (arg == "+k")
				{
					if (this->_key == true)
						{
						server->sendError(client->getFd(), "467", this->_name, "Channel key already set");
						std::cout << "DEBUG ChangeModeK : channel a deja un password" << std::endl;
						}
					else if (this->_key == false && isValidChannelPW(key) == true)
					{
						this->_password = key;
						this->_key = true;
						//message: :<nick>!user@host MODE #channel +k
						// :serveur 324 <nick> #channel +k motdepasse
					}
					else
						{
						std::cout << "DEBUG ChangeModeK : bad channel key - password non valid" << std::endl;
						server->sendError(client->getFd(), "475", this->_name, "Cannot join channel (+k)");
						}
				}
				if (arg == "-k")
				{	
					if (this->_key == true && this->_password == key)\
					{
						this->_key = false;
						//message: :<nick>!user@host MODE #channel -k
						//:serveur 324 <nick> #channel -k
					}
					else if (this->_key == false)	
					{
						std::cout << "DEBUG ChangeModeK : channel est deja en mode -k : message confirmation" << std::endl;
						//message: :<nick>!user@host MODE #channel -k
						//:serveur 324 <nick> #channel -k
					}
					else 
					{
						std::cout << "DEBUG ChangeModeK : mauvais password de channel : mot de passe ignore et mode -k active" << std::endl;
						// attention infos contradictoires trouvees sur le sujet
						//server->sendError(client->getFd(), "475", this->_name, "Cannot join channel (+k)");
						//message: :<nick>!user@host MODE #channel -k
						//:serveur 324 <nick> #channel -k
					}
				}
			}
			else
			{
				std::cout << "DEBUT ChangeModeK : client not an operator" << std::endl;
				server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
			}
	}

void Channel::changeModeO(Server* server, Client* client, std::string arg, Client* cible) 
{
	if (verifClientisOperator(client) == true )
	{
		if (verifClientisInChannel(cible) == false)
		{
			std::cout << "DEBUT ChangeModeO : Cible not in channel" << std::endl;
			server->sendError2(client->getFd(), "441", cible->getNick(), this->_name, "They aren't on that channel");
		}
 		else		
		{
			if (arg == "+o" && verifClientisUser(cible) == true)
			{
				std::cout << "DEBUG ChangeModeO : cible passe de user a operator" << std::endl;
				userToOperator (cible);
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				//:serveur 324 <nick> #canal +o cible
			}
			else if (arg == "+o" && verifClientisUser(cible) == false)
			{
				std::cout << "DEBUT ChangeModeO : cible already operator : envoi message confirmation" << std::endl;
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				//:serveur 324 <nick> #canal +o cible
			}
			else if (arg == "-o" && verifClientisOperator(cible) == true)
			{
				std::cout << "DEBUG ChangeModeO : cible passe de operator a user" << std::endl;
				operatorToUser(cible);
				// message: :<nick>!<user>@<host> MODE #canal -o cible
				//:serveur 324 <nick> #canal -o cible
			}
			else
			{
				std::cout << "DEBUT ChangeModeO : cible not an operator : envoi message confirmation" << std::endl;
				// message: :<nick>!<user>@<host> MODE #canal +o cible
				//:serveur 324 <nick> #canal +o cible
			}
		}
	}
	else
	{
		std::cout << "DEBUT ChangeModeO : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
	}

}


void Channel::changeModeL(Server* server, Client* client, std::string arg, int limit) 
{
	if (verifClientisOperator(client) == true )
	{
		if (arg == "+l" && _hasLimit == false)
		{
			_hasLimit = true;
			_limit = limit;
			// message confirmation : :<nick>!<user>@<host> MODE #canal +l 10
			// :serveur 324 <nick> #canal +l 10
		}
		else if (arg == "+l" && _hasLimit == true)
		{
		_limit = limit;
			std::cout << "DEBUT ChangeModeL : limite modifiee" << std::endl;
			// message confirmation : :<nick>!<user>@<host> MODE #canal +l 10
			//:serveur 324 <nick> #canal +l 10
		}
		else if (arg == "-l" && _hasLimit == true)
		{
			_hasLimit = false;
			//message confirmation : :<nick>!<user>@<host> MODE #canal -l
			//:serveur 324 <nick> #canal -l 
		}
		else 
		{
			std::cout << "DEBUT ChangeModeL : no limit is set - commande ignoree" << std::endl;
			//commande ignoree - aucun message
		}
	}
	else 
	{
		std::cout << "DEBUT ChangeModeL : client not an operator" << std::endl;
		server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
	}
}

void Channel::printUsers() const {
    for (std::vector<Client*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
        std::cout << "       - " << *it << "\n";
    for (std::vector<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
        std::cout << "       - (op) " << *it << "\n";
}


// client : celui qui envoie la commande
// cible : nickname utilise dans la commande
void Channel::commandKick(Server* server, Client* client, Client* cible, std::string comment)
{
	if (verifClientisOperator(client) == true )
	{
		if (verifClientisInChannel(cible) == true)
		{
			removeUser(cible->getFd());
			removeInvited(cible);
			if (comment != "")
			{
				std::cout << "DEBUG commKick : message confirmation sans comment" << std::endl;	
				// message envoye a tous y compris client et cible
				// message confirmation : :<nick>!user@host KICK #channel cible 
			}
			else
				std::cout << "DEBUG commKick : message confirmation avec comment" << std::endl;	
				// message envoye a tous y compris client et cible
				// message confirmation : :<nick>!user@host KICK #channel cible :<comment>
		}
		else
			{
			std::cout << "DEBUG commKick : cible not in channel" << std::endl;
			server->sendError2(client->getFd(), "441", cible->getNick(), this->_name, "They aren't on that channel");

			}
	}
	else
		{
			server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
			std::cout << "DEBUG commKick : client not an operator" << std::endl;
		}
}

// inviter une personne deja invitee ne cree pas d'erreur et le message de confirmnation est envoye
void Channel::commandInvite(Server* server, Client* client, Client* cible)
{
	if (verifClientisOperator(client) == true )
	{
		if (verifClientisInChannel(cible) == true)
		{
			server->sendError(client->getFd(), "443", cible->getNick(), "is already on channel");
			std::cout << "DEBUG commInvite : deja dans channel" << std::endl;
		}
		else 
		{
			addInvited(cible);	
			// message:	:<Nick client>!user@host INVITE <nick cible> :#channel : recu uniquement par bob
			// message:	:server 341 <Nick client> <nicjk cible> #channel : recu uniquement par client
		}
	}
	else
		{
			server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
			std::cout << "DEBUG commInvite : client not an operator" << std::endl;
		}
}

void Channel::commandTopic(Server* server, Client* client, std::string topic)
{
			// if topic est vide : afficher le topic
			if (topic == "")
			{
				// message recu par tous les utilisateurs du canal y compris le demandeur :
				//:<nick client>!user@host TOPIC <#channel> :<topic>
				// pas de message specifique envoye uniquement au demandeur
				std::cout << "DEBUG commtPOPIC : message topic actuel" << std::endl;
			}
			else	if (_topicRestriction == true)
			{
			 	if (verifClientisOperator(client) == true)
				{
					_topicName = topic;
					// message recu par tous les utilisateurs du canal y compris le demandeur :
					//:<nick client>!user@host TOPIC <#channel> :<topic>
					// pas de message specifique envoye uniquement au demandeur
					std::cout << "DEBUG commtPOPIC : message nouveau topic" << std::endl;
				}
				else 
				{
					server->sendError(client->getFd(), "482", this->_name, "You're not channel operator");
					std::cout << "DEBUG commtPOPIC : client not an operator" << std::endl;
				}
			}
			else
			{
				_topicName = topic;
				// message recu par tous les utilisateurs du canal y compris le demandeur :
				//:<nick client>!user@host TOPIC <#channel> :<topic>
				// pas de message specifique envoye uniquement au demandeur
			}
		}


//---- DEBUG ZENABA 
void Channel::ChannelSend(const std::string& message, Client* sender)
{
    for (std::vector<Client*>::iterator it = _users.begin(); it != _users.end(); ++it)
    {
        if (*it != sender)
            (*it)->send_msg(message);
    }
}

void Channel::printClientVectors() const {
	std::cout << "  [_users] Clients:\n";
	for (std::vector<Client*>::const_iterator it = _users.begin(); it != _users.end(); ++it)
		std::cout << "    - ptr: " << *it << " (" << (*it)->getNickname() << ")\n";

	std::cout << "  [_operators] Clients:\n";
	for (std::vector<Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
		std::cout << "    - ptr: " << *it << " (" << (*it)->getNickname() << ")\n";
}

	
int Channel::getClientCount() const 
{
	std::set<Client*> uniqueClients; // stl set pas de doublon

	// Ajouter tous les user
	uniqueClients.insert(_users.begin(), _users.end());
	// Ajouter operator (pour poas ajouter deux fois)
	uniqueClients.insert(_operators.begin(), _operators.end());
	return uniqueClients.size();
}