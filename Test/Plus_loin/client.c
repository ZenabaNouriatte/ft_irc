#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define CONNEXION_HOST "127.0.0.1"
#define LISTENING_PORT 5095
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
    char buffer[BUFFER_SIZE] = {0};
    while (1)
    {
        // Saisie du message à envoyer
        printf("Client > ");
        fgets(buffer, BUFFER_SIZE, stdin);
        int sentBytes = send(socketFd, buffer, strlen(buffer), 0);
        if (sentBytes == -1)
        {
            fprintf(stderr, "Client - fail send to server\n");
            break;
        }

        if (strncmp(buffer, "exit", 4) == 0)
            break;

        // Réception de la réponse du serveur
        memset(buffer, 0, BUFFER_SIZE);
        int receivedBytes = recv(socketFd, buffer, BUFFER_SIZE, 0);
        if (receivedBytes <= 0)
        {
            fprintf(stderr, "Client - server disconnected or error\n");
            break;
        }

        printf("Serveur : %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }
    close(socketFd);

    return 0;
}