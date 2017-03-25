
#include "car_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>


CarServer::CarServer(CarState* state, int fd_socket):
  state(state),
  fd_socket(fd_socket) {}

CarServer::~CarServer(){
  delete server_thread;
}

void CarServer::Start(){
  server_thread = new std::thread(&CarServer::SyncronizeState, this);
  server_thread->detach();
}

void CarServer::SyncronizeState(){

  char msg[60];
  int msglen=39;

  struct sockaddr_in s;

  s.sin_family = AF_INET;
  s.sin_port = htons(5000);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  while(true){
    if(state->new_message){
      state->new_message = false;
      std::cout<<state->message<<"\n";
      snprintf(msg,60,"Am primit mesajul %s",state->message.data());
      msglen = 60;
      sendto(fd_socket,msg,msglen,0,(struct sockaddr *)&s, sizeof(struct sockaddr_in));
    }
  }
}
