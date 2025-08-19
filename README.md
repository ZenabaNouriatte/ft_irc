# Serveur IRC temps réel en C++ – Projet ft_irc

## Présentation du projet

Ce projet implémente un serveur IRC minimaliste en C++ conforme au protocole IRC RFC 1459 (et certaines extensions courantes). L'objectif est de gérer la communication entre plusieurs clients via des sockets non-bloquants et le mécanisme **poll()**, en respectant les formats de commandes et la gestion des canaux.

Le serveur est capable de :
- Accepter plusieurs connexions simultanées
- Envoyer un message de bienvenue à chaque client connecté
- Gérer les commandes IRC de base (`PASS`, `NICK`, `USER`, `JOIN`, `PRIVMSG`, `PING`...)
- Créer dynamiquement des channels et permettre aux clients de communiquer entre eux

## 🖥Fonctionnement

### 1. Lancement du serveur

```bash
./ircserv <port> <password>
```

### 2. Connexion d'un client (exemple avec `nc`)

```bash
nc 127.0.0.1 <port>
```

### 3. Authentification

```irc
PASS <password>
NICK monPseudo
USER monPseudo 0 * :Mon Nom
```

### 4. Rejoindre un canal et discuter

```irc
JOIN #canal
PRIVMSG #canal :Bonjour à tous !
```

## En image ...

1. **Connexion d'un client et message de bienvenue**
![Connexion et message de bienvenue](screenshots/Screenshot%20from%202025-08-19%2009-58-18.png)

2. **Communication entre deux clients dans un channel**
![Deux clients communiquant dans un channel](screenshots/Screenshot%20from%202025-08-19%2010-01-14.png)

## 🛠Explication du code

Le projet est organisé en plusieurs classes principales :

### `Server`
- Initialise le socket d'écoute
- Gère les événements réseau avec `poll()`
- Accepte les nouvelles connexions et délègue la gestion des messages via une boucle principale

### `Client`
- Contient les informations d'un utilisateur (socket, pseudo, état d'authentification)
- Gère un buffer pour les messages partiels


### `Channel`
- Représente un canal IRC
- Gère la création des channels, l’ajout d’utilisateurs et l’utilisation des modes
- Stocke la liste des clients connectés


### Mise en oeuvre de la connexion 
- Description du cycle accept → enregistrement → message de bienvenue
- Gestion du non-bloquant et des événements POLLIN / POLLOUT
- Politique de fermeture/déconnexion propre

### Gestion des commandes
- Chaque commande IRC (`JOIN`, `PRIVMSG`, `PING`...) est traitée par une fonction dédiée
- **Exemple :** `handleJoin()` parse les arguments, crée un channel si nécessaire, et y ajoute le client

##  Axes d'amélioration

### Gestion d'erreurs avec `try`/`catch`
- Remplacer la propagation silencieuse d'erreurs par des exceptions contrôlées
- Permettrait d'éviter la fermeture brutale du serveur en cas de problème réseau ou d'instruction invalide
