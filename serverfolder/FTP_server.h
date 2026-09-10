#include <stdio.h>
#include "csapp.h"

#define MAX_NAME_LEN 256
#define NB_PROC 5


typedef enum { GET } typereq_t;

typedef struct {
    char nom_fichier[MAX_NAME_LEN];
    typereq_t type_requete;
} request_t;

void run_server(int argc, char **argv);

int inTableau(pid_t pid ,pid_t * tableau);

void handler_SIGCHILD(int sig);