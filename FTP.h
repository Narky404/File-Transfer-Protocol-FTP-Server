#include <stdio.h>
#include "csapp.h"

#define MAX_NAME_LEN 256
#define NB_PROC 5
#define PORT 1212
#define BLOCK_SIZE 20


typedef enum { GET, BYE } typereq_t;

typedef struct {
    char nom_fichier[MAX_NAME_LEN];
    typereq_t type_requete;
} request_t;

void run_server(int argc, char **argv);

void handler_SIGCHILD(int sig);

void handler_SIGINT(int sig);

void handler_SIGPIPE(int sig);
