#include "car_connection.h"
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

CarConnection::CarConnection(CarState* state, std::pair<const std::string, const int> serverAddress):
  state(state),
  serverAddress(serverAddress){}

void CarConnection::Connect(){
  struct sockaddr_in SocketClient;
  memset((char*) &SocketClient, 0, sizeof(SocketClient));
  std::cout << "Se creaza socketul" << std::endl;

  fd_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(fd_socket == -1) {
    std::cout << "Nu am putut crea socketul!" << std::endl;
  }

  SocketClient.sin_family = AF_INET;
  SocketClient.sin_port = htons(serverAddress.second);
  SocketClient.sin_addr.s_addr=inet_addr(serverAddress.first.data());
  int broadcastEnable = 1;
  int ret = setsockopt(fd_socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

  if(bind(fd_socket, (const sockaddr*) &SocketClient, sizeof(SocketClient))==-1) {
    close(fd_socket);
    std::cout << "Nu am putut bindui socketul!" << std::endl;
  }
}

void CarConnection::Start(){
  client = std::unique_ptr<CarClient>(new CarClient(state, fd_socket));
  server = std::unique_ptr<CarServer>(new CarServer(state, fd_socket, serverAddress.second));

  client->Start();
  server->Start();
}

CarConnection::~CarConnection(){
    std::cout<<"SOCKET IS CLOSING!"<<std::endl;
    close(fd_socket);
}
