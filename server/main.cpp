#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdint.h>

#include "float_pack.hpp"

/*
static int bunny_color_id = 0;
static int const max_players = 3;
static int num_players = 0;
static struct sockaddr_in6 clients[3];
*/

static volatile float bunny_yrot = 0.f;

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

  // TODO: Create thread that updates physics
  //pthread_t physics_thread;

  // listen for clients and process as necessary
  // TODO: only gives "go run" commands to clients
  int n;
  struct sockaddr_in6 client_info;
  unsigned int client_info_len;
  uint32_t server_message = 1;
  uint32_t client_message;

  // Sends bunny rotation information to whoever requests it
  int count = 0;
  while(1) {
    n = recvfrom(player_socket, &client_message, sizeof(uint32_t), 0,
      (struct sockaddr *) &client_info, &client_info_len);
    server_message = pack754_32(bunny_yrot);
    n = sendto(player_socket, &server_message, sizeof(uint32_t), 0,
      (struct sockaddr *) &client_info, client_info_len);
    // Update bunny rotation
    bunny_yrot = bunny_yrot + .00001f;
    if(bunny_yrot > 6.28f)
      bunny_yrot = 0.f;
    printf("bunny_yrot: %f\n", bunny_yrot);
  }

  return 0;
  
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
