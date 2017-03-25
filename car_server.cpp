
#include "car_server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
  std::cout << "Am pornit sincronizarea starii la server!";

  char msg[60];
  int msglen=39;

  while(true){
    if(state->new_message){
      state->new_message = false;
      std::cout<<state->message<<"\n";
      std::cout<<"Am primit!";
      snprintf(msg,60,"Am primit mesajul %s",state->message.data());
      msglen = 60;
      send(fd_socket,msg,msglen,0);
    }
  }
}
