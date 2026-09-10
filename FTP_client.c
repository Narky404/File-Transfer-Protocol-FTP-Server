#include "FTP.h"

int main(int argc, char **argv)
{
    int j=0;
    double elapsed_time;
    int clientfd;
    char *host;
    char buf[MAX_NAME_LEN], filename[MAX_NAME_LEN*2];
    rio_t rio;
    ssize_t check_crash;
    clock_t start, end;
    int recup_crash;
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    
    /*
    * Note that the 'host' can be a name or an IP address.
    * If necessary, Open_clientfd will perform the name resolution
    * to obtain the IP address.
    */
   clientfd = Open_clientfd(host, PORT);
   
   /*
   * At this stage, the connection is established between the client
   * and the server OS ... but it is possible that the server application
   * has not yet called "Accept" for this connection
   */
  printf("client connected to server OS\n"); 
  
  Rio_readinitb(&rio, clientfd);
  while(1) {
        recup_crash = 0;
        int i;
        char path[] = "./clientfolder/";
        path[strlen(path)]='\0';
        char *fichier;
        request_t request;
        request.nom_fichier[0] = '\0';
        filename[0]='\0';
        buf[0]='\0';

        printf(">>> ");
        Fgets(buf, MAX_NAME_LEN, stdin); //recup la commande    
        
        strcpy(filename, buf);

        fichier = &filename[4];

        // recup le nom du fichier
        for(i = 0; fichier[i] != '\0'; i++) {
            request.nom_fichier[i] = fichier[i];
        }
        request.nom_fichier[i] = '\0';
        filename[3]='\0'; // on garde que la commande maintenant
        // on cherche a savoir de quel type c'est
        if ( strcmp(filename, "get")==0 || strcmp(filename, "GET")==0 ) {
            request.type_requete = GET;
        } else if(strcmp(filename, "bye") == 0 || strcmp(filename, "BYE") == 0) { //si l'utilisateur tape bye, on ferme la connexion
            request.type_requete = BYE;
        }else{ // si commande inconnue
            fprintf(stderr, "Unknown command.\n");
            continue;
        }
        
        //printf("request.nom_fichier = %s\n", request.nom_fichier[0] == '\0' ? "yes" : "no");
        Rio_writen(clientfd, &request, sizeof(request_t)); // envoie la commande au serveur
        
        
        if(request.type_requete == BYE){
            printf("Goodbye.\n");
            Close(clientfd);
            exit(0);
        }
        // gestion cas où le client ne donne pas de nom de fichier
        if(request.nom_fichier[0] == '\0'){
            fprintf(stderr, "Usage : get (GET) [file_name]\n");
            continue;
        }

        int fichier_existe =0;
        if(Rio_readnb(&rio, &fichier_existe, sizeof(int)) >= 0) { // recevoir message pour savoir si le fichier existe ou pas
            if(fichier_existe){
                fprintf(stderr, "File not found on server.\n");
                continue;
            }
        }
        
        long int taille_fichier = 0;
        if((check_crash = Rio_readnb(&rio, &taille_fichier, sizeof(long int))) <= 0) { //lecture de la taille du fichier
            fprintf(stderr, "Error to read number of blocs.\n");
            return -1;
        }
    
        printf("Size of file to receive : %ld bytes.\n", taille_fichier);

        start = clock(); //debut du timer
        
        char *buffer = malloc(BLOCK_SIZE*sizeof(char)); //allocation de la memoire pour le fichier
        if(buffer == NULL) {
            fprintf(stderr, "Error: malloc failed.\n");
            return 0;
        }

        strcat(path, fichier); //creation du chemin
        int fd = open(path, O_CREAT | O_WRONLY, 0644); //ouverture du fichier pour ecrire dedans
        FILE *fdlog = fopen("log.txt", "r+"); //ouverture du fichier de log pour lire/ecrire dedans
        if (fdlog == NULL) {
            fdlog = fopen("log.txt", "w+");
        }
        if (fd < 0) {
            fprintf(stderr, "Error : impossible to open.\n");
        } else {
            long int taille_fichier_recu = 0;
            printf("Receiving file...\n");
            char* nom_fichier_crash = malloc(MAX_NAME_LEN*sizeof(char));
            if (fdlog != NULL) {
                rewind(fdlog);
                if (fgets(nom_fichier_crash, MAX_NAME_LEN, fdlog) == NULL) {
                    nom_fichier_crash[0] = '\0';
                }
                printf("nom_fichier_crash = %s\n", nom_fichier_crash);
                if(strcmp(nom_fichier_crash, request.nom_fichier) == 0){
                    printf("Detection recup !\n");
                    recup_crash = 1;
                    char* temp = malloc(MAX_NAME_LEN*sizeof(char));
                    if (fscanf(fdlog, "%s", temp) == 1) {
                        taille_fichier_recu = atoi(temp);
                    } else {
                        taille_fichier_recu = 0;
                    }
                    fseek(fdlog, 0, SEEK_SET);
                    //envoie flag pour que le serv sache si c'est c'est une recup ou pas
                    Rio_writen(clientfd, &recup_crash, sizeof(int));
                    // envoie taille_fichier_recu
                    Rio_writen(clientfd, &taille_fichier_recu, sizeof(long int));   
                    free(nom_fichier_crash);
                    free(temp);
                } else {
                    Rio_writen(clientfd, &recup_crash, sizeof(int));
                    taille_fichier_recu = 0;
                }
            } else {
                // Fichier log n'existe pas, pas de récupération
                Rio_writen(clientfd, &recup_crash, sizeof(int));
                taille_fichier_recu = 0;
            }

            lseek(fd, taille_fichier_recu, SEEK_SET);
            // boucle de reception des blocs du fichier
            while(taille_fichier_recu < taille_fichier){
                buffer[0] = '\0';
                // on surveille la taille de bloc ça nous aide a gerée le cas du derniere bloc le "reste"
                long int bytes_a_lire = (taille_fichier - taille_fichier_recu < BLOCK_SIZE) ? (taille_fichier - taille_fichier_recu) : BLOCK_SIZE;
                int tmp = Rio_readnb(&rio, buffer, bytes_a_lire);
                printf("bytes lu : %d\n", tmp);
                if (tmp <= 0){
                    fprintf(stderr, "Error read.\n");
                    return -1;
                }
                Rio_writen(fd, buffer, (ssize_t) tmp); //ecriture dans le fichier de taille tmp
                printf("nb bloc recu : %d\n", j);
                j++;
                taille_fichier_recu += tmp;
                //printf("taille_fichier_recu == %ld\n", taille_fichier_recu);
                // Sauvegarde dyna pour si crash
                if (fdlog != NULL) {
                    rewind(fdlog);
                    fprintf(fdlog, "%s\n%ld\n", request.nom_fichier, taille_fichier_recu);
                    fflush(fdlog);
                }
                //sleep(1);
            }
            //calcul du temps de transfert
            end = clock();
            elapsed_time = (double)(end - start) / CLOCKS_PER_SEC;
            printf("\nFile received in %f seconds.\n", elapsed_time);
            Close(fd);
            if (fdlog != NULL) fclose(fdlog);
        }
        free(buffer);  
    }
}