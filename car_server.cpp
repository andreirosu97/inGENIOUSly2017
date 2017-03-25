
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


CarServer::CarServer(CarState* state, int socket):
  state(state),fd_socket(socket),
  serverAddress(serverAddress){}

CarServer::~CarServer(){
  std::cout<<"CLOSING CAR SERVER!\n";
  close(fd_socket);
}

void CarServer::Start(){
  ServerThread = new std::thread(&CarServer::SyncronizeState, this);
  ServerThread->detach();
}

void CarServer::SyncronizeState(){

  char msg[40];
  int msglen=39;

  while(true){
    if(state->new_message){
      state->new_message = false;
      std::cout<<state->message<<"\n";
      snprintf(msg,60,"Am primit mesajul %s",state->message.data());
      send(fd_socket,msg,msglen,0);
    }
  }
}
