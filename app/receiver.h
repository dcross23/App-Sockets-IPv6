#ifndef __RECEIVER_H__
#define __RECEIVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <net/if.h>

#define MAX_SIZE_MSG    4096
#define MAX_SIZE_IF     25
#define MAX_SIZE_GROUP  255

#define DEFAULT_GROUP   "ff15::33"
#define DEFAULT_IFACE   "eth0"
#define DEFAULT_PORT    4343

#define TAM(x) (sizeof(x))

typedef struct sockaddr_in6 Socket6info;

typedef struct ReceiverArgs{
	char multicastGroup[MAX_SIZE_GROUP];
	char iface[MAX_SIZE_IF];
	int port;
}ReceiverArgs;


int func_multicastReceiver(ReceiverArgs *rArgs);
int checkArgs(int argc, char **argv, ReceiverArgs *rArgs);

//Handler for Control+C
void SIGINT_handler(int signal);

#endif