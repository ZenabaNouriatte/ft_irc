#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define LISTENING_PORT 5095
#define PENDING_QUEUE_MAXLENGHT 1
#define BUFFER_SIZE 1024

int main ()
{
    // creation du socket
    int socketFd = socket ( AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1)
    {
        fprintf(stderr, "Serveur - fail init socket\n");
        exit(1);
    }
    // config du socket 
    struct sockaddr_in socketAddress;

    socketAddress.sin_family = AF_INET ;
    socketAddress.sin_port= LISTENING_PORT;
    socketAddress.sin_addr.s_addr = INADDR_ANY;

    int socketAdresseLenght= sizeof(socketAddress);
    int bindReturn = bind (socketFd, (struct sockaddr*) &socketAddress, socketAdresseLenght);
    if (bindReturn  == -1)
    {
        fprintf(stderr, "Serveur - fail bind\n");
        exit(1);
    }
    // Attente de nouvelles connexions
    if (listen(socketFd, PENDING_QUEUE_MAXLENGHT) == -1)
    {
        fprintf(stderr, "Serveur - fail listen\n");
        exit(1);
    }
    printf("En attente de nouvelle connexion...\n");



    int connectedSocketFD = accept(socketFd, (struct sockaddr*)&socketAddress,
                                (socklen_t*) &socketAdresseLenght);
    if ( connectedSocketFD == -1 )
    {
        fprintf(stderr, "Serveur - echec etablissement connexion\n");
        exit(1);
    }
    // Reception d'un msg
    char buffer[BUFFER_SIZE] = {0};

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int receivedBytes = recv(connectedSocketFD, buffer, BUFFER_SIZE, 0);
        if (receivedBytes == -1)
        {
            fprintf(stderr, "Serveur - echec reception du ;sg du client\n");
            exit(1);
        }
        printf("Client : %s\n", buffer);
        if (strncmp(buffer, "exit", 4) == 0)
            break;

        printf("Serveur > ");
        fgets(buffer, BUFFER_SIZE, stdin); // lire la saisie utilisateur
        int sentBytes = send(connectedSocketFD, buffer, strlen(buffer), 0);
        if (sentBytes == -1)
        {
            fprintf(stderr, "Serveur - fail envoi message\n");
            break;
        }
        if (strncmp(buffer, "exit", 4) == 0)
            break;
    }
    //Fermeture des socket
    close(connectedSocketFD);
    close(socketFd);

    return 0;
}