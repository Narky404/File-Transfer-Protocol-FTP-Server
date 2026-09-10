#include "csapp.h"

#define PORT 1212

int main(int argc, char **argv)
{
    long int size;
    int clientfd;
    char *host;
    char buf[MAXLINE], filename[MAXLINE];
    char path[MAXLINE] = "./clientfolder/";
    int fd;
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
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

    for(int i =0; i<3; i++ ) { //envoi de 3 messages, la commande, la taille du nom du fichier, et le nom du fichier
        Fgets(buf, MAXLINE, stdin);
        Rio_writen(clientfd, buf, strlen(buf));
    }
    strcpy(filename, buf);

    Rio_readnb(&rio, &size, sizeof(long int)); //lecture de la taille du fichier
    printf("size : %ld\n", size);

    char* buffer = malloc(size*sizeof(char));
    Rio_readnb(&rio, buffer, size); //lecture du fichier
    printf("buffer : %s\n", buffer);
    strcat(path, filename);
    printf("path : %s\n", path);
    fd = open(path, O_CREAT | O_WRONLY);
    write(fd, buffer, size);
    free(buffer);
    Close(clientfd);
    exit(0);
}