
#include "car_server.h"

#include <cstring>

CarServer::CarServer(CarState* state, int fd_socket):
  state(state),
  fd_socket(fd_socket) {}

CarServer::~CarServer(){
  thread_on=0;
  server_thread->join();
  std::cout<<"CLOSING SERVER!"<<std::endl;
}

void CarServer::Start(){
  s.sin_family = AF_INET;
  s.sin_port = htons(5001);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  server_thread = new std::thread(&CarServer::SyncronizeState, this);

}

void CarServer::SendMessage(char* message) {
  sendto(fd_socket, message, strlen(message), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in));
}


void CarServer::SyncronizeState(){
  struct sockaddr_in s;
  while(thread_on){
    unsigned char telegrama[6];
    state->get_my_state(telegrama);
    SendMessage((char*)telegrama);

    usleep(100000);
  }
}
