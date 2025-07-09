#include "../includes/Server.hpp"
// #include "../includes/Client.hpp"
// #include "../includes/Channel.hpp"

int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port < 1024 || port > 65535) 
    {
        std::cerr << "Error: Port number must be between 1024 and 65535." << std::endl;
        return (1);
    }
    std::string password = argv[2];
    Server server(port, password);
    server.start();

    return 0;
}
