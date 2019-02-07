#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
static int bunny_color_id = 0;
static int const max_players = 3;
static int num_players = 0;
static struct sockaddr_in6 clients[3];
*/

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "usage: ./server [port number]\n");
    exit(1);
  }

  int player_socket;
  struct sockaddr_in6 serv_info;

  player_socket = socket(AF_INET6, SOCK_DGRAM, 0);
  if(player_socket < 0) {
    fprintf(stderr, "error occurred while opening player socket\n");
    exit(1);
  }

  // set server information
  memset(&serv_info, 0, sizeof(serv_info));
  serv_info.sin6_family = AF_INET6;
  serv_info.sin6_port = htons(atoi(argv[1]));
  serv_info.sin6_addr = in6addr_any;

  // bind server information to socket
  if(bind(player_socket, (struct sockaddr *) &serv_info,
    sizeof(serv_info)) < 0) {
    fprintf(stderr, "error occurred while binding socket\n");
    exit(1);
  }

  // listen for clients and process as necessary
  // TODO: only gives "go run" commands to clients
  int n;
  struct sockaddr_in6 client_info;
  unsigned int client_info_len;
  int client_message, server_message = htonl(-1);
  while(1) {
    n = recvfrom(player_socket, &client_message, sizeof(int), 0,
      (struct sockaddr *) &client_info, &client_info_len);
    n = sendto(player_socket, &server_message, sizeof(int), 0,
      (struct sockaddr *) &client_info, client_info_len);
  }

  return 1;
  
  {
    // get first message from the player
    int n;
    struct sockaddr_in6 client_info;
    unsigned int client_info_len = sizeof(struct sockaddr_in6);
    int client_message, server_message = -1;
    printf("waiting for message...\n");
    n = recvfrom(player_socket, &client_message, sizeof(int), 0,
      (struct sockaddr *) &client_info, &client_info_len);
    // TESTING START
    char buf[40];
    inet_ntop(AF_INET6, &(client_info.sin6_addr.s6_addr),
      buf, 40);
    printf("got message from %s\n", buf);
    // TESTING END
    while(1) {
      n = sendto(player_socket, &server_message, sizeof(int), 0,
        (struct sockaddr *) &client_info, client_info_len);
      sleep(1);
    }
  }
}
