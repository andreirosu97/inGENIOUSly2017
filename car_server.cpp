
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
  struct sockaddr_in s;

  s.sin_family = AF_INET;
  s.sin_port = htons(5001);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  while(true){
    if(state->new_message){
      state->new_message = false;
      state->message= state->message + " BINGO\n";
      std::cout<<"\n Am trimis: " << state->message<< "\n";
      sendto(fd_socket,state->message.data(),state->message.size(),0,(struct sockaddr *)&s, sizeof(struct sockaddr_in));
    }
  }
}
