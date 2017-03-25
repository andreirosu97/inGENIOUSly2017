#include "car_connection.h"
#include "car_client.h"
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

CarConnection::CarConnection(CarState* state, std::pair<const std::string, const int> serverAddress):
  state(state),
  serverAddress(serverAddress){}

void CarConnection::Connect(){
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

  if(bind(fd_socket, (const sockaddr*) &SocketClient, sizeof(SocketClient))==-1) {
    close(fd_socket);
    //throw udp_client_server_runtime_error("Could not bind UDP socket with: \n" );
    std::cout << "Nu am putut bindui socketul!!" << std::endl;
  }
}

void CarConnection::Start(){
  CarClient* client = new CarClient(state, fd_socket);
  CarServer* server = new CarServer(state, fd_socket);

  client->Start();
  server->Start();

  // GOD IS THE ONLY OWNER OF THESE OBJECTS
}

CarConnection::~CarConnection(){
    std::cout<<"CLOSING SOCKET!\n";
    close(fd_socket);
}
