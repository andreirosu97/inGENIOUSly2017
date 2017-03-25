
#include "car_client.h"
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


CarClient::CarClient(CarState* state, std::pair<const std::string, const int> serverAddress):
  state(state),
  serverAddress(serverAddress){}

void CarClient::Connect(){
  struct sockaddr_in SocketClient;
  memset( (char*) &SocketClient ,0 ,sizeof(SocketClient));
  std::cout << "NE CONECTAM!!" << std::endl;

  fd_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(fd_socket == -1) {
  //  throw udp_client_server_runtime_error("Could not create UDP socket!\n");
    std::cout << "Nu am putut crea socketul!!" << std::endl;
  }

  SocketClient.sin_family = AF_INET;
  SocketClient.sin_port = htons(serverAddress.second);
  SocketClient.sin_addr.s_addr=inet_addr(serverAddress.first.data());

  if(bind( fd_socket, (const sockaddr*) &SocketClient, sizeof(SocketClient))==-1) {
    close(fd_socket);
    //throw udp_client_server_runtime_error("Could not bind UDP socket with: \n" );
    std::cout << "Nu am putut bindui socketul!!" << std::endl;
  }
}

CarClient::~CarClient(){
    std::cout<<"CLOSING CLIENT!\n";
    close(fd_socket);
}

void CarClient::Start(){
  std::thread SyncronizeStateThread(&CarClient::SyncronizeState, this);
  SyncronizeStateThread.join();
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
