#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
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
  serv_info.sin6_port = htons(atoi("9001"));
  serv_info.sin6_addr = in6addr_any;

  // bind server information to socket
  if(bind(player_socket, (struct sockaddr *) &serv_info,
    sizeof(serv_info)) < 0) {
    fprintf(stderr, "error occurred while binding socket\n");
    exit(1);
  }

  // get first message from the player
  int n;
  struct sockaddr_in6 client_info;
  unsigned int client_info_len = sizeof(struct sockaddr_in6);
  int client_message, server_message = -1;
  printf("waiting for message...\n");
  n = recvfrom(player_socket, &client_message, sizeof(int), 0,
    (struct sockaddr *) &client_info, &client_info_len);
  printf("got message\n");
  while(1) {
    n = sendto(player_socket, &server_message, sizeof(int), 0,
      (struct sockaddr *) &client_info, client_info_len);
    sleep(1);
  }
}
