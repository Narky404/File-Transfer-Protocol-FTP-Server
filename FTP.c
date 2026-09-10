
/*
 * FTP
 */
#include "FTP.h"

void ftp(int connfd){

    long int taille_fichier;
    rio_t rio;
    ssize_t check_crash;

    Rio_readinitb(&rio, connfd);
    while(1) {
        char path[] = "./serverfolder/";
        request_t request;
        //reset buffer du nom du fichier 
        request.nom_fichier[0] = '\0';
        
        // serveur en attente d'une commande de la part du client
        printf("Waiting for a command\n");
        if ( ( check_crash = Rio_readnb(&rio, &request, sizeof(request_t)) ) == 0){ //recupere la commande et si y a une erreur de lecture
            fprintf(stderr, "Error: read command line.\n");
            return;
        }

        if(request.type_requete == BYE){
            exit(0);
        }
        // le client ne sait pas utiliser la commande get et on veut pas que le serveur continue dans l'execution alors on l'exit
        if(request.nom_fichier[0] == '\0'){
            continue;
        }

        strcat(path, request.nom_fichier); //creation du chemin
        //determine la taille du fichier
        path[strlen(path)-1]='\0';
        int fd = open(path, O_RDONLY);
        int fichier_absent = 0;
        if (fd < 0){
            fprintf(stderr, "Error : file not found.\n");
            fichier_absent = 1;
        }
        rio_writen(connfd, &fichier_absent, sizeof(int)); //envoi taille du fichier (0 dans ce cas)
        if (fichier_absent) {
            continue;
        }

        char* buffer;
        struct stat statbuf;
        int file_info = fstat(fd, &statbuf);
        if (file_info < 0){
            fprintf(stderr, "Error: file not found.\n");
            return;
        } else {
            taille_fichier = statbuf.st_size;

            printf("File size to send = %ld bytes.\n", taille_fichier);
            
            Rio_writen(connfd, &taille_fichier, sizeof(long int)); //envoi taille du fichier
            buffer = malloc(taille_fichier *sizeof(char));
            if(buffer == NULL) {
                fprintf(stderr, "Error: malloc failed.\n");
                return;
            }

            int taille_fichier_envoye;
            int recup_crash;

            // recup valeur pour savoir si c'est une recuperation ou pas
            Rio_readnb(&rio, &recup_crash, sizeof(int));
            if(recup_crash){
                printf("Client wants to recover a crash !\n");
                Rio_readnb(&rio, &taille_fichier_envoye, sizeof(long int)); //lecture de la taille du fichier qui a été recu avant le crash
            } else {
                taille_fichier_envoye = 0;
            }
            
            //taille_fichier_envoye = 80;
            lseek(fd, taille_fichier_envoye, SEEK_SET); //positionnement du curseur de lecture du fichier a la position de la taille du fichier qui a été recu avant le crash
            
            // boucle d'envoi des blocs du fichier vers client
            while(taille_fichier_envoye != taille_fichier){
                int tmp = Rio_readn(fd, buffer, (ssize_t) BLOCK_SIZE); //recuperation du contenu du fichier de taille BLOCK_SIZE
                Rio_writen(connfd, buffer, (ssize_t) BLOCK_SIZE); //envoi du contenu du fichier de taille BLOCK_SIZE
                taille_fichier_envoye += tmp;
            }

            Close(fd);
            printf("File successfully sent.\n");
            free(buffer);
        }
    }
}