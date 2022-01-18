#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define READSIZE 1500
#define DATASIZE 512
#define DATAPACKETSIZE 516
#define ACKSIZE 4
char * service="69";//port sur la machine srvtpinfo1.ensea.fr

//getaddrinfo(node,service,hints,res);
//node = adresse
//service = n°port
//hints = forme sous laquelle on veut recevoir res
//res = structure addrinfo qui correponds au serveur


//RECIPROQUE : getnameinfo()

void * RRQpacket(char * filename, char * mode) {
	int filename_size = strlen(filename);
	int mode_size = strlen(mode);
	int opcode_size = 2;
	
	int size = opcode_size + filename_size + 1 + mode_size + 1;
	
	char * paquet=malloc(size);
	
	//Opcode RRQ
	paquet[0]=0;
	paquet[1]=1;
	//filename RRQ
	strcpy(&paquet[2],filename);
	paquet[opcode_size+filename_size]=0;
	//mode RRQ
	strcpy(&paquet[opcode_size+filename_size+1],mode);
	paquet[opcode_size+filename_size+1+mode_size]=0;
	
	return paquet;
}

void * ACKpacket(char * data_recv){
	char * paquet=malloc(ACKSIZE);
	
	//Opcode ACK
	paquet[0]=0;
	paquet[1]=4;
	//Block #
	paquet[2]=data_recv[2];
	paquet[3]=data_recv[3];
	
	return paquet;
}

int main(int argc, char *argv[]){
	struct addrinfo *res;
	struct addrinfo hints;
	char * node=argv[1]; //adresse du serveur
	char * filename=argv[2];
	
	if (argc <3){
		fprintf(stderr, "Usage : pas assez d'argument. Le %s a besoin de 2 arguments supplémentaire : le serveur : %s puis le nom du fichier : %s\n",argv[0],node,filename);
		exit(EXIT_FAILURE);
	}
	
	//appel a getaddrinfo
	memset(&hints,0,sizeof(hints)); // initialise hints
	hints.ai_family=AF_INET; //IPV4
	hints.ai_socktype = SOCK_DGRAM;// Mode datagramme -> UDP
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_UDP; //connexion au serveur doit se faire en UDP
	
	if (getaddrinfo(node,service,&hints,&res)){
		fprintf(stderr, "Error : %s n'a pas réussi à récupérer l'adresse et le port du serveur\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	//Pour tester la commande getaddrinfo
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	getnameinfo(res->ai_addr,res->ai_addrlen,host,sizeof(host),port,sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
	printf("Adresse IP du serveur : %s\nPort : %s\n",host,port);
	
	//Réservation d'un socket de connexion vers le serveur
	int sfd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	
	if (sfd==-1){
		fprintf(stderr, "Error : %s n'a pas réussi à se connecter vers le serveur %s\n",argv[0],argv[1]);
		exit(EXIT_FAILURE);
	}
	
	//envoie d'une requête RRQ
	char * mode ="octet";
	char * request=RRQpacket(filename,mode);
	int size_request=2+strlen(filename)+1+strlen(mode)+1;
	int is_send=sendto(sfd,request,size_request,0,res->ai_addr,res->ai_addrlen);
	if (is_send==-1){
		fprintf(stderr, "Error : %s n'a pas réussi à senvoyer une demande RRQ au serveur %s\n",argv[0],argv[1]);
		exit(EXIT_FAILURE);
	}
	
	//récupération de la data
	char data_recv[READSIZE];
	char hbuf[NI_MAXHOST];
	char sbuf[NI_MAXSERV];
	struct sockaddr src_addr;
	socklen_t src_addrlen = sizeof(src_addr);
	
	int nread = DATAPACKETSIZE;
	
	while(nread==DATAPACKETSIZE){
		nread=recvfrom(sfd,data_recv,READSIZE,0,&src_addr,&src_addrlen);
		write(STDOUT_FILENO, data_recv, nread);
		//envoie d'un acquittement
		char * acquitement=ACKpacket(data_recv);
			//vérifie qu'il envoit des ACK
		getnameinfo(&src_addr,src_addrlen,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST|NI_NUMERICSERV);
		fprintf(stdout,"Sending ACK to: %s:%s | nread : %d\n",hbuf,sbuf,nread);
		int is_acq=sendto(sfd,acquitement,ACKSIZE,0,&src_addr,src_addrlen);
		if (is_acq==-1){
			fprintf(stderr, "Error : %s n'a pas pu envoyer un acquittement\n",argv[0]);
			exit(EXIT_FAILURE);
		}

	}
	
	exit(EXIT_SUCCESS);
}


