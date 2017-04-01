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
  delete client_thread;
}

void CarClient::Start(){
  client_thread = new std::thread(&CarClient::SyncronizeState, this);
  client_thread->detach();
}

void CarClient::SyncronizeState(){
  /*Initializing variables */
  fd_set fd_reading;
  char msg[60]="Mesaj!";
  int max_size = 60;
  struct timeval timeout;
  FD_ZERO(&fd_reading);
  FD_SET(fd_socket, &fd_reading);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;

  /*Reading loop*/
  int retval;

  while(true){
    retval = select(fd_socket+1, &fd_reading, NULL ,NULL , &timeout);
    if(retval == -1){
      std::cout<<"Select error!";return;
    }else if(retval ==0){
      std::cout<<"Server timed out!\n";return;
    }
    else{
        int recv_size = recv(fd_socket, msg, max_size, 0);
        msg[recv_size] = '\0';
        std::cout<<"Receptionat: "<<msg<<"!\n";
        state->update_message(std::string(msg));
        if(std::string(msg) == "STOP")
          state->shut_down();
    }
  }
  return;
}
