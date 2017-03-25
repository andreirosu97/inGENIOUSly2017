
#include "car_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <ctime>
#include <cstdlib>


CarClient::CarClient(State state, std::pair<const std::string, const int> serverAddress):
  state(state),
  serverAddress(serverAddress){}

void CarClient::Connect(){
  struct sockaddr_in SocketClient;
  memset( (char*) &SocketClient ,0 ,sizeof(SocketClient));

  fd_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  //if(fd_socket == -1)
  //  throw udp_client_server_runtime_error("Could not create UDP socket!\n");

  SocketClient.sin_family = AF_INET;
  SocketClient.sin_port = htons(serverAddress.second);
  SocketClient.sin_addr.s_addr=inet_addr(serverAddress.first.data());

  if(bind( fd_socket, (const sockaddr*) &SocketClient, sizeof(SocketClient))==-1) {
    close(fd_socket);
    //throw udp_client_server_runtime_error("Could not bind UDP socket with: \n" );
  }
}

CarClient::~CarClient(){
    std::cout<<"CLOSING SERVER!\n";
    close(fd_socket);
}

void CarClient::Start(){
  std::thread SyncronizeStateThread(&CarClient::SyncronizeState, this);
  SyncronizeStateThread.detach();
}

void CarClient::SyncronizeState(){

  fd_set fd_reading;
  FD_ZERO(&fd_reading);
  FD_SET(fd_socket, &fd_reading);
  struct timeval timeout;
  timeout.tv_sec = 60;
  timeout.tv_usec = 0;
  int retval = select(fd_socket+1, &fd_reading, NULL ,NULL , &timeout);

  if(retval == -1) {
    std::cout<<"Select error!";
    }else if(retval==0) {
      std::cout<<"Server timed out!\n";
      return;
      } else {
        recv(f_socket, msg, max_size, 0);
        std::cout<<msg<<"\n";
        timed_recv(msg,max_size,max_wait_sec);
  }
}
