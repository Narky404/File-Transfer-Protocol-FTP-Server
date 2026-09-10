/*
 * echo - read and echo text lines until client closes connection
 */
#include "FTP_server.h"

void echo(int connfd)
{
    int i = 0;
    rio_t rio;
    request_t request;
    char buf[MAX_NAME_LEN]; /* tableau de char */
    char cmd[MAX_NAME_LEN]; /* stock la commande */
    int file_info;
    int fd;
    struct stat statbuf;
    long int size;

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAX_NAME_LEN); /* identifie la commande utilisé */
    printf("%s", buf);
        for (i = 0; buf[i] != '\n'; i++){
            cmd[i] = buf[i];
        }
        //printf("cmd = %s\n", cmd);
        //printf("filename = %s\n", request.nom_fichier);
        if(strcmp(cmd, "GET") == 0) {
            request.type_requete = GET;
        }    
        
    Rio_readlineb(&rio, buf, MAX_NAME_LEN); /* recupere la taille du nom du fichier */
    printf("taille : %s", buf);
    size = (long int) buf;
    
    Rio_readlineb(&rio, buf, size);  /* recupere nom du fichier */
        for (i = 0; buf[i] != '\n'; i++ ){
            request.nom_fichier[i] = buf[i];
        }
        printf("nom fichier %s\n", request.nom_fichier);
    
    /* trouver la taille du fichier*/
        fd = open(request.nom_fichier, O_RDONLY);
        if (fd < 0){ 
            printf("Error : impossible to open\n");
        }

        file_info = fstat(fd, &statbuf);        
        if (file_info < 0){
            printf("Error: file not found\n");
            return;
        } else {
            size = statbuf.st_size;
            printf("taille : %ld\n", size);
            Rio_writen(connfd, &size, sizeof(size));
            char* buffer = malloc(size*sizeof(char));
            read(fd, buffer, size);
            printf("bufferserver : %s\n", buffer);
            Rio_writen(connfd, buffer, size);
            free(buffer);
            Close(fd);
        }


        

        //printf("server received %u bytes\n", (unsigned int)n);
        //Rio_writen(connfd, buf, n);
}


