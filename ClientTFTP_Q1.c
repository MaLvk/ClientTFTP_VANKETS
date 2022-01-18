#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * service="69";//port sur la machine srvtpinfo1.ensea.fr

int main(int argc, char *argv[]){
	char * node=argv[1]; // adresse serveur
	char * filename=argv[2];
	
	if (argc <3){
		fprintf(stderr, "Usage : pas assez d'argument. Le %s a besoin de 2 arguments supplémentaire : le serveur : %s puis le nom du fichier : %s\n",argv[0],node,filename);
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
