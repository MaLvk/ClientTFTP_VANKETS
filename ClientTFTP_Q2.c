#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
char * service="69";//port sur la machine srvtpinfo1.ensea.fr

//getaddrinfo(node,service,hints,res);
//node = adresse
//service = n°port
//hints = forme sous laquelle on veut recevoir res
//res = structure addrinfo qui correponds au serveur


//RECIPROQUE : getnameinfo()

int main(int argc, char *argv[]){
	struct addrinfo *res;
	struct addrinfo hints;
	char * node=argv[1]; //adresse du serveur
	char * filename=argv[2];
	
	if (argc <3){
		fprintf(stderr, "Usage : pas assez d'argument. Le %s a besoin de 2 arguments supplémentaire : le serveur : %s puis le nom du fichier : %s\n",argv[0],node,filename);
		exit(EXIT_FAILURE);
	}
	
	memset(&hints,0,sizeof(hints)); // initialise hints
	hints.ai_family=AF_INET; //IPV4
	hints.ai_socktype = SOCK_DGRAM;// Mode datagramme -> UDP
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_UDP; //connexion au serveur doit se faire en UDP
	
	if (getaddrinfo(node,service,&hints,&res)){
		fprintf(stderr, "Usage : %s n'a pas fonctionné correctement\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//Pour tester la commande getaddrinfo
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	getnameinfo(res->ai_addr,res->ai_addrlen,host,sizeof(host),port,sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
	printf("Adresse IP du serveur : %s\nPort : %s\n",host,port);
	
	exit(EXIT_SUCCESS);
}
