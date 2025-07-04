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
    std::string password = argv[2];

    Server server(port, password);

    return 0;
}
