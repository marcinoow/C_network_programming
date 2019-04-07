/* File:         tcp_echo_cli.c
 * Authors:      Marcin ********
 * Date:         29.03.2019
 * Description:  Program is an example of a simple tcp echo client 
 *               for IPv4.
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
#include <stdbool.h>

#define MAX_MSG_LEN 4096
#define SERV_PORT "8888"
#define SERV_IP "127.0.0.1"


/**************************** FUNCTIONS *******************************/

/**
 * Function print help for user.
 */
void print_info();


/**
 * Function send recived data.
 * 
 * @param fd is a client socket descriptor
 */
void echo_func(int fd);

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

  const int serv_socket = socket(AF_INET, SOCK_STREAM, 0);
  if ( serv_socket < 0 ) {
    fprintf(stderr, "ERROR socket(): %s\n", strerror(errno));
    exit(2);
  }

  socklen_t len = sizeof(serv_addr);

  if ( bind(serv_socket, (struct sockaddr*) &serv_addr, len) < 0 ) {
    fprintf(stderr, "ERROR bind(): %s\n", strerror(errno));
    exit(3);
  }

  /* Start listening on the socket. */
  if ( listen(serv_socket, 10) ) {
    fprintf(stderr, "ERROR listen(): %s\n", strerror(errno));
    exit(4);
  }

  printf("Waiting for connection... \n");

  while( 1 ) {  

    /* Address structure for the client socket. */
    struct sockaddr_in cli_addr = {};
    socklen_t lenc = sizeof(cli_addr);

    /* Get connection. */
    const int cli_socket = accept(serv_socket, (struct sockaddr*) &cli_addr, &lenc);
    if ( cli_socket < 0 ) {
      fprintf(stderr, "ERROR accept(): %s\n", strerror(errno));
	  continue;
    }
    
    printf("Connection accepted.\n");

    /* Start echo loop. */
    echo_func(cli_socket);
    
    close(cli_socket);
  }

  return 0;
}


/*********************** FUNCTIONS DEFINITIONS ************************/

void print_info() {
	printf("Usage: tcp_echo_serv -[OPTION] [VALUE]\n");
	printf("       tcp_echo_serV -[OPTION]\n");
	printf("       tcp_echo_serv -[OPTION]... -[OPTION] [VALUE]...\n");
	printf("\n");
	printf("Options:\n");
	printf("       -h            show this help\n");
	printf("       -i [ip]       set ip address (default ip is \"127.0.0.1\")\n");
	printf("       -p [socket]   set port to listen (default port is \"8888\")\n");
	printf("\n");
	printf("\n");
}


void echo_func(int fd) {
	
  /* Data buffer. */
  char buffer[MAX_MSG_LEN];
  
  /* Number of characters recived. */
  int n;
    
  /* Send recived data. */
  while ( (n = read(fd, buffer, sizeof(buffer))) > 0 ) {
    if ( write(fd, buffer, sizeof(buffer)) <= 0 ) {
      fprintf(stderr, "ERROR write(): %s\n", strerror(errno));
    }
  }
  if (n < 0) {
	fprintf(stderr, "ERROR read(): %s\n", strerror(errno));
  }
}

