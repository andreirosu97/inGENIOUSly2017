
#include "car_server.h"

#include <cstring>

CarServer::CarServer(CarState* state, int fd_socket):
  state(state),
  fd_socket(fd_socket) {}

CarServer::~CarServer(){
  delete server_thread;
}

void CarServer::Start(){
  s.sin_family = AF_INET;
  s.sin_port = htons(5001);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  server_thread = new std::thread(&CarServer::SyncronizeState, this);
  server_thread->detach();
}

void CarServer::SendMessage(char* message) {
  sendto(fd_socket, message, strlen(message), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in));
}


void CarServer::SyncronizeState(){
  struct sockaddr_in s;
  while(true){
    unsigned char telegrama[6];
    telegrama[0] = 0x01;
    telegrama[1] = 0x08;
    telegrama[2] = 0xff;
    telegrama[3] = state->cars_states[8].first;
    telegrama[4] = state->cars_states[8].second;
    telegrama[5] = 0x00;
    SendMessage((char*)telegrama);
    usleep(100000);
  }
}
