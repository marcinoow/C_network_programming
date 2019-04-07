/* File:         tcp_serv.c
 * Authors:      Marcin ********
 * Date:         29.03.2019
 * Description:  Program is an example of a simple tcp client for IPv4.
 */
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

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
		printf("Option needs a value.\n");
		exit(1);
	  case '?':
		fprintf(stderr, "Unknown option: %c.\n", optopt);
		exit(1);	
	} 
  }

  /* Address structure for the client socket. */
  struct sockaddr_in cli_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(atoi(serv_port))
  };

  /* Convert IPv4 address from text to binary form. */
  if( inet_pton(AF_INET, serv_ip, & cli_addr.sin_addr) <= 0 ) {
    fprintf(stderr, "ERROR inet_pton(): %s\n", strerror(errno));
    exit(1);
  }

  /* Create client socket. */
  const int cli_socket = socket(AF_INET, SOCK_STREAM, 0);

  if( cli_socket < 0 ) {
    fprintf(stderr, "ERROR socket(): %s\n", strerror(errno));
    exit(2);
  }

  /* Calculate size of client address structure. */
  socklen_t len = sizeof(cli_addr);

  /* Connect with server. */
  if( connect(cli_socket,(struct sockaddr*) &cli_addr, len) < 0 ) {
    fprintf(stderr, "ERROR connect(): %s\n", strerror(errno));
    return -1;
  }

  /* Start echo loop. */
  echo_func(cli_socket);
  
  /* Close socket. */
  close(cli_socket);
  
  return 0;  
}


/*********************** FUNCTIONS DEFINITIONS ************************/

void print_info() {
	printf("Usage: tcp_echo_cli -[OPTION] [VALUE]\n");
	printf("       tcp_echo_cli -[OPTION]\n");
	printf("       tcp_echo_cli -[OPTION]... -[OPTION] [VALUE]...\n");
	printf("\n");
	printf("Options:\n");
	printf("       -h            show this help\n");
	printf("       -i [ip]       set ip address (default ip is \"127.0.0.1\")\n");
	printf("       -p [socket]   set port (default port is \"8888\")\n");
	printf("\n");
	printf("\n");
}


void echo_func(int fd) {	
  int n;

  char sendline[MAX_MSG_LEN];
  char recvline[MAX_MSG_LEN];
  
  while(1) {
    bzero(sendline, MAX_MSG_LEN);
    bzero(recvline, MAX_MSG_LEN);

    printf("Enter text:\n");
    fgets(sendline, MAX_MSG_LEN, stdin);
    printf("\n");
  
    /* Send data to server. */
    if ( write(fd, sendline, strlen(sendline)+1) <= 0 ) {
      fprintf(stderr, "ERROR write(): %s\n", strerror(errno));
      exit(6);
    }

    /* Get data from server. */
    if ( (n = read(fd, recvline, sizeof(recvline))) <= 0 ) {
      fprintf(stderr, "ERROR read(): %s\n", strerror(errno));
      exit(5);
    }
 
    /* Print recived data. */
    printf("Recived:\n%s\n\n", recvline);
  
  }	
}
