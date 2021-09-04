#ifndef __SENDER_H__
#define __SENDER_H__

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

#define DEFAULT_MSG      "Hola que tal"
#define DEFAULT_GROUP    "ff15::33"
#define DEFAULT_IFACE    "eth0"
#define DEFAULT_PORT     4343
#define DEFAULT_HOPS     10
#define DEFAULT_INTERVAL 5

#define SIZE(x) (sizeof(x))

typedef struct sockaddr_in6 Socket6info;

typedef struct SenderArgs{
	char msg[MAX_SIZE_MSG];
	char multicastGroup[MAX_SIZE_GROUP];
	char iface[MAX_SIZE_IF];
	int port;
	int hops;
	int interval;
}SenderArgs;


int func_multicastSender(SenderArgs *sArgs);
int checkArgs(int argc, char **argv, SenderArgs *sArgs);

//Handler for Control+C
void SIGINT_handler(int signal);

#endif