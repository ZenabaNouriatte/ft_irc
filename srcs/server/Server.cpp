#include "Server.hpp"

int Server::signal = 0;

Server::Server(int port, const std::string& password) : _port(port), _password(password), _server_fd(-1)
{
    std::cout << "Arg[1] = port : " << this->_port
                << "\nArg[2] = password : " << this->_password << std::endl;
}

Server::~Server() 
{
    cleanExit();
    std::cout << "DEBUG Server destroyed\n";
}

void Server::handleError(const std::string& message) 
{
    std::cerr << message << std::endl;
    if (_server_fd >= 0)
        close(_server_fd);
}

void Server::start()
{
    _server_fd = socket(AF_INET, SOCK_STREAM,0); // creer un socket : famille d'adresse ipv4 , type TCP , protocole par default (tcp)
    if (_server_fd == -1)
        return handleError ("Fail socket\n");

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        return handleError("Fail setsockopt SO_REUSEADDR\n");
    //pour relancer le server avec le meme port
    fcntl(_server_fd, F_SETFL, O_NONBLOCK);  // socket non bloquant garanti que accept recv ne bloquerons pqs 

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    socklen_t f_addr_len = sizeof(addr);

    if (bind(_server_fd, (sockaddr*)&addr, f_addr_len) == -1)
        return handleError ("Fail bind\n");
    // associe adresse ip et port 
    if (listen(_server_fd, 5)  == -1)
        return handleError ("Fail listen\n");
    std::cout << "Server ready on port: " << _port << std::endl;
    // construire les pollfd pour chaque socket a surveiller 
    pollfd pfd;
    pfd.fd = _server_fd;
    pfd.events = POLLIN;
    _poll_fds.push_back(pfd);


    while (Server::signal == 0)
    {
        // Appel de poll() avec un tableau de pollfd
        int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 1000);
        if (poll_count < 0)
        {
            if (errno == EINTR)
                continue; // en cas de ctrc c pour ne pas que polle failed 
            return handleError("poll failed");
        }

        for (size_t i = 0; i < _poll_fds.size(); ++i) 
        {
            // Examiner revents pour chaque socket
            if (_poll_fds[i].revents & POLLIN) 
            {
                if (_poll_fds[i].fd == _server_fd) 
                {
                    acceptNewClient();
                }
                else 
                {
                    handleClient(_poll_fds[i].fd);
                }
            } 
            else if (_poll_fds[i].revents & (POLLHUP | POLLERR)) 
            {
                removeClient(_poll_fds[i].fd);
                i--; //modifi le vector
            }
        }
        if (Server::signal != 0)
            break;
    }
}

void Server::acceptNewClient()
{
    sockaddr_in client_addr; // stock les info client qui se connect
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t addr_len = sizeof(client_addr); //taille

    int client_fd = accept(_server_fd, (sockaddr*)&client_addr, &addr_len); // on rempli la structure client_adrr avec les information trouve via accept
    if (client_fd == -1)
        return handleError("Fail accept");
    std::cout << "DEBUG : New client connected on fd: " << client_fd << std::endl;
    
    // Mettre le socket client en mode non bloquant
    fcntl(client_fd, F_SETFL, O_NONBLOCK);

    //On ajoute une entree pollfd pour ce client dans _poll_fds pour que poll() suveille le socket
    pollfd pfd;
    pfd.fd = client_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _poll_fds.push_back(pfd);

    Client *client = new Client(client_fd); // on creer un nouveau client via son fd
    _clients[client_fd] = client; //stock le client dans le map avec cles = fd

}

void Server::handleClient(int client_fd)
{
    char buffer[1024]; // recv attend un tableau de char , 1024 taille suffisament grand pour un msg
    ssize_t received_bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (received_bytes > 0)
    {
        buffer[received_bytes] = '\0';  // Termine proprement la chaîne
        std::cout << "[RECV fd=" << client_fd << "] " << buffer << std::endl;

        // Ajouter donnee au buffer client
        Client* client = _clients[client_fd];
        client->appendToBuffer(buffer); // copier buffer de rcv dans _buffer.client

        // Traiter les lignes completes qui termine par \r\n
        std::string& buf = client->getBuffer(); // reference pour acceder a _buffer et non faire une copie
        size_t pos =buf.find("\r\n");
        while (pos != std::string::npos)
        {
            std::string raw_message = buf.substr(0, pos); // recupere la prochaine ligne
            buf.erase(0, pos + 2); // supprime la ligne + \r\n

            // Creation obj msg
            Message msg(raw_message);
            pos = buf.find("\r\n"); // mettre à jour pos a la fin
            //msg.parsing(); //ici parsing

        }

    }
    else if (received_bytes == 0)
    {
        std::cout << "[INFO] Client " << client_fd << " a fermé la connexion." << std::endl;
        removeClient(client_fd);
    }
    else
    {
        handleError("[ERROR] recv()");
        removeClient(client_fd);
    }
}


void Server::removeClient(int client_fd)
{
    std::cout << "DEBUG Supp du client " << client_fd << std::endl;

    // Fermer la socket
    close(client_fd);

    //Supprimer le pollfd associé
    for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
    {
        if (it->fd == client_fd)
        {
            _poll_fds.erase(it);
            break;
        }
    }

    // Supprimer le client de la map + memoire
    std::map<int, Client*>::iterator it = _clients.find(client_fd);
    if (it != _clients.end())
    {
        delete it->second;
        _clients.erase(it);
    }
}

void Server::catchSignal(int signum)
{
    signal = signum;
}
void Server::cleanExit() 
{
    for (size_t i = 0; i < _poll_fds.size(); ++i)
        close(_poll_fds[i].fd);

    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete it->second;

    _poll_fds.clear();
    _clients.clear();

    std::cout << "DEBUG Propre exit ...\n";
}


	void Server::suppressChannel(const Channel & channel)  
	{
		if (channel.isChannelEmpty() == 0)
	{
		// supprimer le channel du vecteur channel du serveur
	}
	}
	