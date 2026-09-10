## 1. Introduction

L'objectif de ce mini-projet est de développer un **serveur FTP simplifié**, permettant à un client de demander un fichier au serveur, qui le lui enverra s'il existe. Cette première étape met en place les fonctionnalités de base pour la transmission de fichiers en utilisant une communication via **sockets**.
---

## 2. Architecture

Le projet est structuré en trois parties principales :
- **Client** : Fichier `FTP_client.c`
- **Serveur** : Fichier `FTP_server.c`
- **Protocole FTP** : Fichier `FTP.c` contenant les instructions pour le protocole.

Un fichier d'en-tête **`FTP.h`** définit :
- Le numéro de port (1212, modifié pour éviter les conflits avec les machines universitaires).
- Le nombre de processus dans le *pool*.
- La structure `requete_t` pour les requêtes (nom de fichier, type de commande : `GET` ou `bye`).

---

## 3. Implémentation

### Squelette du code
Basé sur un modèle classique **client-serveur**, adapté pour le FTP.

#### **Serveur**
- Écoute sur le **port 1212**.
- Utilise un **pool de processus** pour gérer plusieurs clients simultanément.
- Gère la **terminaison propre** des processus via le signal **SIGINT**.

#### **Client**
- Envoie une requête **GET** au serveur, suivie :
  1. Du nombre de caractères dans le nom du fichier.
  2. Du nom du fichier.
- Reçoit le fichier en réponse.

---

### Gestion des fichiers
- **Côté serveur** : Les fichiers sont stockés dans le répertoire `serverfolder/`.
- **Côté client** : Les fichiers reçus sont enregistrés dans `clientfolder/`.

---

### Transmission des données

#### **Envoi en un seul bloc**
1. **Serveur** :
   - Charge le fichier demandé en mémoire.
   - Si le fichier est introuvable, renvoie un **code d'erreur**.
   - Envoie la **taille du fichier**, puis son contenu en un seul bloc.

2. **Client** :
   - Envoie la commande, la taille du nom du fichier, puis le nom du fichier.
   - Reçoit la taille du fichier, puis son contenu.
   - Crée un fichier dans `clientfolder/` et y écrit le contenu reçu.

#### **Envoi en plusieurs blocs**
1. **Serveur** :
   - Vérifie la présence du fichier dans `serverfolder/`.
   - Calcule le **nombre de blocs** nécessaires (taille fixe) et la taille du **dernier bloc** (potentiellement partiel).
   - Envoie :
     - Le nombre de blocs.
     - La taille du dernier bloc.
     - Le contenu **bloc par bloc**.

2. **Client** :
   - Crée un fichier dans `clientfolder/`.
   - Reçoit le nombre de blocs, la taille du dernier bloc, puis le contenu bloc par bloc.
   - Écrit les blocs dans le fichier.

**Note** : Le client peut demander plusieurs fichiers lors d'une même connexion et terminer la communication avec la commande **"bye"**.

---

### Traitement des erreurs
- **Vérification systématique** : À chaque envoi/réception, le serveur et le client vérifient la bonne communication.
- **Arrêt en cas d'erreur** : Si un problème est détecté (ex. : client crash), la communication s'arrête.
- **Limite** : La gestion du signal **SIGPIPE** (lorsqu'un client crash) n'a pas été implémentée.

---
---

## Conclusion

Ce mini-projet a permis de :
1. Implémenter un serveur FTP basique avec une requête **GET**.
2. Améliorer le serveur pour gérer **plusieurs connexions simultanées** via un pool de processus.
3. Optimiser la transmission des fichiers en les découpant en **blocs**, pour une meilleure gestion des fichiers volumineux.

---
