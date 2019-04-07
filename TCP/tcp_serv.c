/* File:         tcp_serv.c
 * Authors:      Marcin ********
 * Date:         22.03.2019
 * Description:  Program is an example of a simple tcp server for IPv4.
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
	/* Case for "server -i -p". */
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
  const int serv_socket = socket(AF_INET, SOCK_STREAM, 0);
  if ( serv_socket < 0 ) {
    fprintf(stderr, "ERROR socket(): %s\n", strerror(errno));
    exit(2);
  }

  /* Calculate size of server address structure. */
  socklen_t len = sizeof(serv_addr);

  /* Assigns a local protocol address to a socket. */
  if ( bind(serv_socket, (struct sockaddr*) &serv_addr, len) < 0 ) {
    fprintf(stderr, "ERROR bind(): %s\n", strerror(errno));
    exit(3);
  }

  /* Start listening on the socket. */
  if ( listen(serv_socket, 10) ) {
    fprintf(stderr, "ERROR listen(): %s\n", strerror(errno));
    exit(4);
  }

  /* Data buffer. */
  char buffer[MAX_MSG_LEN];
  
  /* Number of characters recived. */
  int n;
    

  /* Wait for connection. */
  while( 1 ) {
	  
    printf("Waiting for connection...\n");

    /* Address structure for the client socket. */
    struct sockaddr_in cli_addr = {};

    /* Variable with size of client address structure. */
    socklen_t lenc = sizeof(cli_addr);

    /* Get connection. */
    const int cli_socket = accept(serv_socket, (struct sockaddr*) &cli_addr, &lenc);
    if ( cli_socket < 0 ) {
      fprintf(stderr, "ERROR accept(): %s\n", strerror(errno));
	  continue;
    }

    /* Get data from client. */
    if ( (n = read(cli_socket, buffer, sizeof(buffer))) <= 0 ) {
      fprintf(stderr, "ERROR read(): %s\n", strerror(errno));
      exit(5);
    }
    buffer[n] = '\0';
    
    /* Print recived data. */
    printf("Recived:\n%s\n\n", buffer);

    /* Clear buffer and copy message to send to the buffer. */
    bzero(buffer, MAX_MSG_LEN);
    strcpy(buffer, "Hello client!");

    /* Send data. */
    if ( write(cli_socket, buffer, sizeof(buffer)) <= 0 ) {
      fprintf(stderr, "ERROR write(): %s\n", strerror(errno));
      exit(6);
    }
    
    /* Clear buffer. */
    bzero(buffer, MAX_MSG_LEN);

    /* Close client socket. */
    close(cli_socket);
  }

  return 0;
}


/*********************** FUNCTIONS DEFINITIONS ************************/

void print_info() {
	printf("Usage: tcp_serv -[OPTION] [VALUE]\n");
	printf("       tcp_serV -[OPTION]\n");
	printf("       tcp_serv -[OPTION]... -[OPTION] [VALUE]...\n");
	printf("\n");
	printf("Options:\n");
	printf("       -h            show this help\n");
	printf("       -i [ip]       set ip address (default ip is \"127.0.0.1\")\n");
	printf("       -p [socket]   set port to listen (default port is \"8888\")\n");
	printf("\n");
	printf("\n");
}
