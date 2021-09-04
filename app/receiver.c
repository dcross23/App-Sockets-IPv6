#include "receiver.h"

//Global socket so it can be closed in SIGINT handler function
int s;

/**
 * MAIN:
 */
int main(int argc, char **argv){

	ReceiverArgs rArgs;

	//Register SIGINT for CONTROL+C
	struct sigaction sa;
	sa.sa_handler = SIGINT_handler;
	sa.sa_flags = 0;
	
	if(-1 == sigaction(SIGINT, &sa, NULL)){
		perror("Error: sigaction");
		return -1;
	}

	if(-1 == checkArgs(argc, argv, &rArgs)){
		fprintf(stderr, "USO: %s [dirIPv6multicast interfaz puerto]\n", argv[0]);
		return -1;
	}
	
	//Prints params used for the reception
	fprintf(stdout, "PARAMETROS UTILIZADOS\n");
	fprintf(stdout, "---------------------------------------------------------\n");
	fprintf(stdout, "\tGRUPO:    %s\n", rArgs.multicastGroup);
	fprintf(stdout, "\tINTERFAZ: %s\n", rArgs.iface);
	fprintf(stdout, "\tPUERTO:   %d\n", rArgs.port);
	fprintf(stdout, "---------------------------------------------------------\n\n");
	fprintf(stdout, "MENSAJES RECIBIDOS\n");
	fprintf(stdout, "---------------------------------------------------------\n");
	
	
	//Starts receiver 
	return func_multicastReceiver(&rArgs);
}




/**
 * Multicast receiver function
 */
int func_multicastReceiver(ReceiverArgs *rArgs){
								//Socket as global var
	Socket6info receiverInfo;	//Receiver info
	struct ipv6_mreq ipv6mreq;	//IPv6 info to join multicast group
	
	int msgSize;							//Size of the message received
	char msg[MAX_SIZE_MSG];					//Msg received
	Socket6info senderInfo;					//Sender info
	int sock6Infolen = TAM(Socket6info); 	//Tamaño struct sockaddr_in6
	char ipDifusor[INET6_ADDRSTRLEN]; 		//Sender IP as text
	int msgCounter;							//Received messages counter

	
	//Clear structs and buffers
	bzero(&receiverInfo, TAM(receiverInfo));
	bzero(&ipv6mreq, TAM(ipv6mreq));
	bzero(msg, MAX_SIZE_MSG);
	
	
	//Initializes structures
	receiverInfo.sin6_family   = AF_INET6;
	receiverInfo.sin6_port     = htons(rArgs->port);
	receiverInfo.sin6_addr     = in6addr_any;
	receiverInfo.sin6_flowinfo = 0;
	receiverInfo.sin6_scope_id = if_nametoindex(rArgs->iface);
	
	ipv6mreq.ipv6mr_interface = if_nametoindex(rArgs->iface);
	//Get multicast direction converted to binary before assign it to the struct
	if( -1 == inet_pton(AF_INET6, rArgs->multicastGroup, &ipv6mreq.ipv6mr_multiaddr)){
		perror("Error: inet_pton");
		return -1;
	}
	
	
	//Create socket IPv6
	if ( (s = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
		perror("Socket: error al crear el socket\n");
		return -1;
	}
	
	//Enable SO_REUSEADDR to enable multiple receiver in the same computer/machine
	int enable = 1;
	if( -1 == setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))){
		perror("Error: setsockopt -> SO_REUSEADDR");
		return -1;
	}
	
	///Bind IPv6 direction and port to the socket
	if(-1 == bind(s, (struct sockaddr *) &receiverInfo, TAM(receiverInfo))){
		perror("Error: bind");
		return -1;
	}
	
	//Specify multicast group from where datagrams are received (given in args)
	if( -1 == setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &ipv6mreq, TAM(ipv6mreq))){
		perror("Error: setsockopt -> IPV6_ADD_MEMBERSHIP");
		return -1;
	}	
	
	//Infinite loop receiving datagrams until CONTROL+C is pressed
	msgCounter=0;
	while(1){
		if(0 < (msgSize = recvfrom(s, msg, MAX_SIZE_MSG-1, 0, (struct sockaddr *) &senderInfo, &sock6Infolen))){
		
			if( NULL == inet_ntop(AF_INET6, &senderInfo.sin6_addr, ipDifusor, INET6_ADDRSTRLEN)){
				perror("Error: inet_ntop");
				return -1;
			}
		
			fprintf(stdout, "Mensaje %3d: \"%s\"    -> Recibido de: [ %s ]\n",msgCounter++, msg, ipDifusor);
		
		}else if(msgSize == -1){
			perror("Error: recvfrom");
			return -1;
		}
	}
}



/**
 * Checks args
 */
int checkArgs(int argc, char **argv, ReceiverArgs *rArgs){

	if(argc == 4){
		//Valores pasados por la linea de comandos
		strcpy(rArgs->multicastGroup, argv[1]);
		strcpy(rArgs->iface, argv[2]);
		rArgs->port = atoi(argv[3]);		
		return 0;
		
	}else if (argc == 1){
		//Valores por defecto
		strcpy(rArgs->multicastGroup, DEFAULT_GROUP);
		strcpy(rArgs->iface, DEFAULT_IFACE);
		rArgs->port = DEFAULT_PORT;
		return 0;
		
	}else{
		return -1;
	}
}


/**
 * Finishes program when pressing CONTROL+C
 */
void SIGINT_handler(int signal){
	close(s);
	exit(0);
}

