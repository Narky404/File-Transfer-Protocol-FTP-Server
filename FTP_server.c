#include "FTP.h"

void ftp(int connfd);

pid_t TABLEAU_PID[NB_PROC];

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    run_server(argc, argv);
}

void run_server(int argc, char **argv) {
    pid_t pid;
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(0);
    }
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(PORT); /* socket de connexion */

    /* initialisation des gestions des differents signaux*/
    Signal (SIGCHLD, handler_SIGCHILD);
    Signal (SIGINT, handler_SIGINT);    
    
    for (int i = 0; i < NB_PROC; i++){ /* Creation des fils (sans petit fils) et sauvegarde de leurs pid dans un tableau */
        pid = Fork();
        if (pid < 0){
            fprintf(stderr, "Error Fork\n");
            return;
        }
        if (pid == 0){/* fils */
            break; /* il sort du for */
        }else {
            TABLEAU_PID[i] = pid; /* sauvegarde du pid*/
        }
    }
    
    if (pid == 0){
        Signal(SIGINT, SIG_DFL); /* signal SIG CHLD doit etre remis a défault*/
        while (1) { //differents processus fils
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); /* socket de communication */
            Close(listenfd); /* deco socket connexion */

            /* determine the name of the client */
            Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                
            /* determine the textual representation of the client's IP address */
            Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                    INET_ADDRSTRLEN);
                
            printf("server connected to %s (%s)\n", client_hostname,
                    client_ip_string);

            ftp(connfd);

            Close(connfd);
        }
    } else { // processus pere
        while(1){
            //le pere attend de recevoir un signal
        }
    }
}

void handler_SIGCHILD(int sig) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0){  }
    if ( pid == -1 && errno != ECHILD){
        unix_error("Wait PID");
    }
}

void handler_SIGINT(int sig){
    printf("\nServer is shutting down...\n");
    for(int i = 0; i < NB_PROC; i++){
        kill(TABLEAU_PID[i], SIGINT);
    }
    waitpid(-1, NULL, WNOHANG);
    exit(0);
}
