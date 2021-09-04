#include "sender.h"

//Global socket so it can be closed in SIGINT handler function
int s;

/**
 * MAIN:
 */
int main(int argc, char **argv){

	SenderArgs sArgs;
	
	//Register SIGINT for CONTROL+C
	struct sigaction sa;
	sa.sa_handler = SIGINT_handler;
	sa.sa_flags = 0;
	
	if(-1 == sigaction(SIGINT, &sa, NULL)){
		perror("Error: sigaction");
		return -1;
	}

	if(-1 == checkArgs(argc, argv, &sArgs)){
		fprintf(stderr, "USO: %s [msg dirIPv6multicast interfaz puerto saltos intervalo]\n", argv[0]);
		return -1;
	}
	
	//Prints params used for the emision
	fprintf(stdout, "PARAMETROS UTILIZADOS\n");
	fprintf(stdout, "---------------------------------------------------------\n");
	fprintf(stdout, "\tMENSAJE:   %s\n", sArgs.msg);
	fprintf(stdout, "\tGRUPO:     %s\n", sArgs.multicastGroup);
	fprintf(stdout, "\tINTERFAZ:  %s\n", sArgs.iface);
	fprintf(stdout, "\tPUERTO:    %d\n", sArgs.port);
	fprintf(stdout, "\tSALTOS:    %d\n", sArgs.hops);
	fprintf(stdout, "\tINTERVALO: %d\n", sArgs.interval);
	fprintf(stdout, "---------------------------------------------------------\n\n");
	fprintf(stdout, "MENSAJES ENVIADOS\n");
	fprintf(stdout, "---------------------------------------------------------\n");
	
	//Starts sender 
	return func_multicastSender(&sArgs);
}




/**
 * Multicast sender function
 */
int func_multicastSender(SenderArgs *sArgs){
								//Socket as global var
	Socket6info senderInfo;		//Sender info
	Socket6info mGroupInfo;		//Multicast group info
	
	int msgSize;							//Size of the message that is going to be sended
	int sock6Infolen = SIZE(Socket6info); 	//Struct sockaddr_in6 size 
	int ifaceIndex;						 	//Index of the interface used to send data
	int msgCounter;							//Received messages counter
	
	
	//Clear structs and buffers
	bzero(&senderInfo, SIZE(senderInfo));
	bzero(&mGroupInfo, SIZE(mGroupInfo));
	
	//Get iface index
	ifaceIndex = if_nametoindex(sArgs->iface);
	
	//Initializes structures
	senderInfo.sin6_family   = AF_INET6;
	senderInfo.sin6_port     = 0;		 		//Any port
	senderInfo.sin6_addr     = in6addr_any; 	//Any of his IPs
	senderInfo.sin6_flowinfo = 0;
	senderInfo.sin6_scope_id = ifaceIndex;
	
	mGroupInfo.sin6_family = AF_INET6;
	mGroupInfo.sin6_port   = htons(sArgs->port);
	//Get multicast direction converted to binary before assign it to the struct
	if( -1 == inet_pton(AF_INET6, sArgs->multicastGroup, &mGroupInfo.sin6_addr)){
		perror("Error: inet_pton");
		return -1;
	}	
	
	
	//Create socket IPv6
	if ( (s = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
		perror("Socket: error al crear el socket\n");
		return -1;
	}
	
	//Bind IPv6 direction and port to the socket
	if(-1 == bind(s, (struct sockaddr *) &senderInfo, SIZE(senderInfo))){
		perror("Error: bind");
		return -1;
	}

	//Enable IPV6_MULTICAST_IF to set the iface through which is going to send datagrams
	if( -1 == setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifaceIndex, SIZE(ifaceIndex))){
		perror("Error: setsockopt -> IPV6_MULTICAST_IF");
		return -1;
	}
	
	//Enable IPV6_MULTICAST_HOPS to specify the number of hops 
	if( -1 == setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &(sArgs->hops), SIZE(sArgs->hops))){
		perror("Error: setsockopt -> IPV6_MULTICAST_HOPS");
		return -1;
	}
	
	//Infinite loop sending datagrams until CONTROL+C is pressed
	msgCounter=0;
	while(1){
		if(0 < (msgSize = sendto(s, sArgs->msg, MAX_SIZE_MSG-1, 0, (struct sockaddr *)&mGroupInfo, SIZE(mGroupInfo)))){
			fprintf(stdout, "Mensaje %3d: \"%s\"    -> Enviado a: [ %s ]\n",msgCounter++, sArgs->msg, sArgs->multicastGroup);
		
		}else if(msgSize == -1){
			perror("Error: sendto");
			return -1;
		}	
		
		sleep(sArgs->interval);
	}
}



/**
 * Checks args
 */
int checkArgs(int argc, char **argv, SenderArgs *sArgs){
	if(argc == 7){
		//Valores pasados por la linea de comandos
		strcpy(sArgs->msg, argv[1]);
		strcpy(sArgs->multicastGroup, argv[2]);
		strcpy(sArgs->iface, argv[3]);
		sArgs->port = atoi(argv[4]);
		sArgs->hops = atoi(argv[5]);
		sArgs->interval = atoi(argv[6]);		
		return 0;
		
	}else if (argc == 1){
		//Valores por defecto
		strcpy(sArgs->msg, DEFAULT_MSG);
		strcpy(sArgs->multicastGroup, DEFAULT_GROUP);
		strcpy(sArgs->iface, DEFAULT_IFACE);
		sArgs->port = DEFAULT_PORT;
		sArgs->hops = DEFAULT_HOPS;
		sArgs->interval = DEFAULT_INTERVAL;
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

