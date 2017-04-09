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

  int max_size = 60;
  struct timeval timeout;
  FD_ZERO(&fd_reading);
  FD_SET(fd_socket, &fd_reading);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;

  /*Reading loop*/
  int retval;

  while(true){
      char msg[60];
      int recv_size = recv(fd_socket, msg, max_size, 0);
      int speed=0;
      std::string message;
      msg[recv_size] = '\0';
      message=std::string(msg);

      std::cout<<"Receptionat: "<<message<<"!\n";

      std::size_t pos = message.find(" ");

      if(pos<message.size()){// Message of 2 arguments
        std::string string_speed = message.substr (pos);
        message=message.substr (0,pos);
        speed=std::stoi(string_speed);
        state->update_motor_direction(message);
        state->update_motor_speed(speed);
      }
      else//Single argument message
        state->update_motor_direction(message);
      state->get_state();
  }
  return;
}
