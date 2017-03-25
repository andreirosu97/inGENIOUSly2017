
#include "car_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>


CarClient::CarClient(CarState* state, int fd_socket):
  state(state),
  fd_socket(fd_socket){}

CarClient::~CarClient() {
  delete ClientThead;
}

void CarClient::Start(){
  ClientThread = new std::thread(&CarClient::SyncronizeState, this);
  ClientThread->detach();
}

void CarClient::SyncronizeState(){
  /*Initializing variables */
  fd_set fd_reading;
  char msg[500];
  int max_size = 500;
  struct timeval timeout;
  FD_ZERO(&fd_reading);
  FD_SET(fd_socket, &fd_reading);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;

  /*Reading loop*/
  int retval = select(fd_socket+1, &fd_reading, NULL ,NULL , &timeout);
  while(retval>0){
    recv(fd_socket, msg, max_size, 0);
    std::cout<<msg<<"\n";
    state->new_message = true;
    state->message = std::string(msg);
  }

  /*Exit*/
  if(retval == -1){
    std::cout<<"Select error!";
  }else{
    std::cout<<"Server timed out!\n";
  }
  return;
}
