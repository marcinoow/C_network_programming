/* File:         udp_serv.c
 * Authors:      Marcin ********
 * Date:         22.03.2019
 * Description:  Program is an example of a simple udp client for IPv4.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define MAX_MSG_LEN 4096
#define SERV_PORT "8888"
#define SERV_IP "127.0.0.1"


/**************************** FUNCTIONS *******************************/

/**
 * Function print help for user.
 */
void print_info();

/**********************************************************************/


int main(int argc, char **argv) {

  /* Set default server port. */
  char serv_port[] = SERV_PORT;
  
  /* Set default IP address of the server. */
  char serv_ip[] = SERV_IP;
	
  /* Parsing user arguments. */
  int c, prev_ind;

  while ( prev_ind = optind, (c = getopt(argc, argv, ":p:i:h")) != -1 ) {
	/* Case for "client -i -p". */
	if ( optind == prev_ind + 2 && *optarg == '-' ) {
	  c = ':';
	  --optind;
	}
	/* Other cases. */
	switch ( c ) {
	  case 'h':
		print_info();
		exit(1);
	  case 'p':
		strcpy(serv_port, optarg);
		break;
	  case 'i':
		strcpy(serv_ip, optarg);
		break;
	  case ':':
		printf("Option needs a value\n");
		exit(1);
	  case '?':
		fprintf(stderr, "Unknown option: %c.\n", optopt);
		exit(1);	
	} 
  }

  /* Address structure for the server socket. */
  struct sockaddr_in serv_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(atoi(serv_port))
  };
  
  /* Convert IPv4 address from text to binary form. */
  if ( inet_pton(AF_INET, serv_ip, &serv_addr.sin_addr) <= 0 ) {
	fprintf(stderr, "ERROR inet_pton(): %s\n", strerror(errno));
    exit(1);
  }

  /* Create server socket. */
  const int serv_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if ( serv_socket < 0 ) {
    fprintf(stderr, "ERROR socket(): %s\n", strerror(errno));
    exit(2);
  }

  /* Data buffer. */
  char buffer[MAX_MSG_LEN];

  /* Number of characters recived. */
  int n;
  
  /* Variable with size of server address structure. */
  socklen_t lens = sizeof(serv_addr);
  
  /* Clear buffer. */
  bzero(buffer, MAX_MSG_LEN);
  
  /* Copy message to send to the buffer. */
  strcpy(buffer, "Hello world!!!");
  
  /* Send data. */
  if ( sendto(serv_socket, (const char *)buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) & serv_addr, lens) <= 0 ) {
    fprintf(stderr, "ERROR send(): %s\n", strerror(errno));
    exit(5);
  }
  printf("Message sent.\n");
  
  /* Clear buffer. */
  bzero(buffer, MAX_MSG_LEN);
  
  /* Recive data. */
  if ( (n = recvfrom(serv_socket, buffer, MAX_MSG_LEN-1, MSG_WAITALL, (struct sockaddr*) & serv_addr, & lens)) <= 0 ) {
	fprintf(stderr, "ERROR recvfrom(): %s\n", strerror(errno));
    exit(4);    
  }
  buffer[n] = '\0';
  
  /* Print recived data. */
  printf("Recived:\n%s\n\n", buffer);
  
  /* Close server socket. */
  close(serv_socket);
  
  return 0; 
}


/*********************** FUNCTIONS DEFINITIONS ************************/

void print_info() {
	printf("Usage: udp_serv -[OPTION] [VALUE]\n");
	printf("       udp_serv -[OPTION]\n");
	printf("       udp_serv -[OPTION]... -[OPTION] [VALUE]...\n");
	printf("\n");
	printf("Options:\n");
	printf("       -h            show this help\n");
	printf("       -i [ip]       set ip address (default ip is \"127.0.0.1\")\n");
	printf("       -p [socket]   set port (default port is \"8888\")\n");
	printf("\n");
	printf("\n");
}
