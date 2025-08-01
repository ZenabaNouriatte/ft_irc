#include "Channel.hpp"
#include "Server.hpp"
#include "Client.hpp"

std::vector<std::string> Server::splitComma(const std::string &input)
{
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string item;

	while (std::getline(ss, item, ','))
		result.push_back(item);
	return result;
}

void Server::handleSingleJoin(Client *client, const std::string &channelName, const std::string &key)
{
    std::cout << "[DEBUG][JOIN] Handle JOIN request\n";
	std::cout << "[DEBUG]  user    = " << client->getNickname() << "\n";
	std::cout << "[DEBUG]  channel = " << channelName << "\n";
	std::cout << "[DEBUG]  key     = " << (key.empty() ? "(no key)" : key) << "\n";
	// Vérifie que le nom est valide
	if (channelName.empty() || channelName[0] != '#')
	{
		sendError(client->getFd(), "403", channelName, "No such channel");
		return;
	}

	// Cherche le channel
	Channel* chan = findChannel(channelName);

	// Si existe pas, crer
	if (!chan)
	{
		std::cout << "[JOIN] Channel '" << channelName << "' does not exist. Creating it.\n";
		Channel* newChannel = new Channel(channelName);
		_channels.push_back(newChannel);
		chan = newChannel;
	}
	else
	{
		std::cout << "[JOIN] Channel '" << channelName << "' already exists.\n";
	}
	// Si client dans le channel
	if (chan->verifClientisUser(client))
    {
        std::cout << "[JOIN] Client already in channel '" << channelName << "'. Skipping.\n";
        return;
    }
	// Ajouter le client au channel
	chan->addUser(this, client, key);

	// ajouter reponse facon IRC a faire demain

	std::cout << "[DEBUG][JOIN] Client " << client->getNickname()
			  << "[DEBUG] joined channel " << channelName
			  << (key.empty() ? "[DEBUG] (no key)" : "[DEBUG] with key") << ".\n";
    std::cout << GREEN << BOLD << "Client successfully added to channel" << RESET << std::endl;

    std::cout << "[DEBUG] Clients in channel after join:\n";
    chan->printClientVectors();

}

bool Server::parseJoin(const Message &msg, std::string &channel,
	std::string &key)
{
	channel.clear(); // reset
	key.clear();
	const std::string &p0 = msg.params[0]; // nom du channel
	if (msg.params.empty())
		return (false);
    if (p0 == "0"|| p0 == "#0")
    {
        channel = "0";
        return true;
    }
	if (p0.empty() || p0[0] != '#') // nom du channel doit commencer par # donc petite verif
		return (false);
	channel = p0;
	if (msg.params.size() >= 2) // si il y a un deuxieme paramettre c'est le mdp
		key = msg.params[1];
	return (true);
}
std::string toLower(const std::string &str) {
    std::string lower;
    for (size_t i = 0; i < str.size(); ++i)
        lower += std::tolower(str[i]);
    return lower;
}

void Server::handleJOIN(Client *client, const Message &msg)
{
	std::string channel, key, user;
	if (msg.params.empty())
	{
		sendError(client->getFd(), "461", "JOIN", "Not enough parameters");
		return ;
	}
    if (!client->isRegistered())
	{
		sendError(client->getFd(), "451", client->getNickname(), "You have not registered");
        return;
    }
    if (!msg.prefix.empty() && toLower(msg.prefix) != toLower(client->getNickname()))    
    {
        sendError(client->getFd(), "451", client->getNickname(), "prefix different from user nickname");
        return;
    }

    if (msg.params[0] == "0" || msg.params[0] == "#0")
	{
        leaveAllChannels(client);
		return;
	}
    std::vector<std::string> channels = splitComma(msg.params[0]);
	std::vector<std::string> keys;
    if (msg.params.size() > 1)
		keys = splitComma(msg.params[1]);

	for (size_t i = 0; i < channels.size(); ++i)
	{
		std::string chan = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";
		std::string tmpChannel, tmpKey;
		Message msg_chan("JOIN " + chan + " :" + key);
		if (!parseJoin(msg_chan,tmpChannel, tmpKey)) 
		{
			sendError(client->getFd(), "403", chan, "No such channel");
			continue;
		}
        if (ClientChannelCount(client) >= 10)
        {
			std::cout << "Client ["<< client->getFd() <<"] has 10 CHannels\n";
            sendError(client->getFd(), "405", channel, "You have joined too many channels");
            return;
        }
		handleSingleJoin(client, chan, key); // logique unique join
	}
}

void Server::leaveAllChannels(Client *client) 
{
    std::cout << BOLD << "Client [" << client->getFd() << "] leaving all channels\n";
    for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); )
    {
        Channel* chan = *it;

        std::cout << "[DEBUG]  -> Checking if client is in channel: " << chan->getName() << "\n";
        std::cout << "[DEBUG]     client ptr = " << client << "\n";

        std::cout << "[DEBUG]    users in channel:\n";
        chan->printUsers();

        if (chan->verifClientisUser(client) || chan->verifClientisOperator(client))
        {
            std::cout << "[DEBUG]  -> client is present in channel!\n";
            chan->removeUser(client->getFd());
            chan->removeOperator(client);

            std::string partMsg = ":" + client->getPrefix() + " PART " + chan->getName() + " :Leaving all channels\r\n";
            chan->ChannelSend(partMsg, client);

            std::cout << "[DEBUG]  -> left channel " << chan->getName() << "\n";
        }
        int result = chan->getClientCount();  // ou isChannelEmpty()

        std::cout << "[DEBUG] Clients in channel before delete:\n";
        chan->printClientVectors();
        std::cout << "[DEBUG] Get Client Count Result = " << result << std::endl;
        // Supprimer le channel si vide
        if (result == 0)
        {
            std::cout << "[DEBUG]  -> channel " << chan->getName() << " is now empty. Deleting it.\n";
            delete chan;
            it = _channels.erase(it); // erase retourne l'itérateur suivant
        }
        else
        {
            ++it;
        }
    }
}
