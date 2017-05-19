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
  thread_on = 0;
  client_thread->join();
  std::cout<< "CLOSING CLIENT!" << std::endl;
}

void CarClient::Start(){
  client_thread = std::unique_ptr<std::thread>(new std::thread(&CarClient::SyncronizeState, this));
}

void CarClient::SyncronizeState(){
  /*Initializing variables */
  fd_set fd_reading;

  int max_size = 1000;
  struct timeval timeout;
  FD_ZERO(&fd_reading);
  FD_SET(fd_socket, &fd_reading);
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;

  /*Reading loop*/
  int retval;

  while(thread_on){
      char msg[1000];
      int recv_size = recv(fd_socket, msg, max_size, 0);

      int speed=0;
      msg[recv_size] = '\0';

      if(msg[0]!=0x01 && msg[0]!=0x02 && msg[0]!=0x03)
      {
        std::string message;
        message=std::string(msg);
        std::cout<<"Receptionat: "<<message<<std::endl;
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
      }
      else
        state->update_continental(msg);

      //state->get_state();
  }
  return;
}
