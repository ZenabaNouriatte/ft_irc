#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define CONNEXION_HOST "127.0.0.1"
#define LISTENING_PORT 5094
#define BUFFER_SIZE 1024

int main ()
{
    int socketFd = socket ( AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        fprintf(stderr, "Client - fail init socket\n");
        exit(1);
    }
    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET ;
    socketAddress.sin_port= LISTENING_PORT;
    int inetReturnCode = inet_pton(AF_INET, CONNEXION_HOST, &socketAddress.sin_addr);
    if (inetReturnCode == -1)
    {
        fprintf(stderr, "Client - fail invalid adress\n");
        exit(1);
    }
    int socketAdresseLenght= sizeof(socketAddress);
    int connexionStatus = connect(socketFd, (struct sockaddr*) &socketAddress, socketAdresseLenght);
    if (connexionStatus  == -1)
    {
        fprintf(stderr, "Client - fail server connexion fail\n");
        exit(1);
    }
    const char *msg = "Bonjour serveur je suis client.\n";
    send(socketFd, msg, strlen(msg), 0);
    
    char buffer[BUFFER_SIZE] = {0};
    int receivedBytes = recv(socketFd, buffer, BUFFER_SIZE, 0);
    if (receivedBytes == -1)
    {
        fprintf(stderr, "Client- fail msg recu server\n");
        exit(1);
    }
    printf("Serveur : %s\n", buffer);
    close(socketFd);

    return 0;
}