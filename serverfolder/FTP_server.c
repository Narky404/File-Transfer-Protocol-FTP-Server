#include "FTP_server.h"

void echo(int connfd);

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
    pid_t tableau_PID[NB_PROC];
    //printf("PERE : %d\n", getpid());
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    char path[MAXLINE] = "./serverfolder/";
    
    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(0);
    }
    port = 1212;
    
    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port); /* socket de connexion */

    Signal( SIGCHLD, handler_SIGCHILD);
    /* TODO Signla pour SIGINT et SIGPIPE*/

    for (int i = 0; i < NB_PROC; i++){ /* Creation des fils (sans petit fils) et sauvegarde de leurs pid dans un tableau */
        pid = Fork();
        if (pid == 0){/* fils */
            break; /* il sort du for */
        }else {
            tableau_PID[i] = pid; /* sauvegarde du pid*/
        }
    }
    
    if (pid == 0){
        //printf("je suis ton fils\n");
        Signal(SIGINT, SIG_DFL); /* signale SIG CHLD doit etre remis a défault*/
    
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

            echo(connfd);

            Close(connfd);
        }
    } else { // processus pere
        //printf("je suis le pere\n");
        while(1){
            /* on evite les zombies */
        }
    }
}

int inTableau(pid_t pid ,pid_t * tableau){
    for (int i = 0; i < NB_PROC; i++){
        //printf("%d pid\n", tableau[i]);
        if (tableau[i] == pid){
            return 1;
        }
    }
    return 0;
}

void handler_SIGCHILD(int sig) {
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0){  }
    if ( pid == -1 && errno != ECHILD){
        unix_error("Wait PID");
    }
}