
#include "car_client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

CarClient::CarClient(State state, pair<std::string, int> serverAddress):
  state{state},
  serverAddress{serverAddress}{}

CarClient::Connect(){
  struct sockaddr_in SocketClient;
  memset( (char*) &SocketClient ,0 ,sizeof(SocketClient));

  fd_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
  if(fd_socket == -1)
    throw udp_client_server_runtime_error("Could not create UDP socket!\n");

  SocketClient.sin_family = AF_INET;
  SocketClient.sin_port = htons(serverData.second);
  SocketClient.sin_addr.s_addr=htonl(serverData.first);

  if(bind( fd_socket, (const sockaddr*) &SocketClient, sizeof(SocketClient))==-1) {
    close(fd_socket);
    throw udp_client_server_runtime_error("Could not bind UDP socket with: \n" );
  }
}

CarClient::~CarClient(){
    std::cout<<"CLOSING SERVER!\n";
    close(fd_socket);
}

CarClient::Start(){
  std::thread SyncronizeStateThread(&CarClient::SyncronizeState,this);
  SyncronizeStateThread.detatch();
}

CarCliet::SyncronizeState(){

  fd_set s;
  FD_ZERO(&s);
  FD_SET(f_socket, &s);
  struct timeval timeout;
  timeout.tv_sec = ;
  timeout.tv_usec = 0;
  int retval = select(f_socket+1, &s, NULL ,NULL , &timeout);

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
