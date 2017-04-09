
#include "car_server.h"

CarServer::CarServer(CarState* state, int fd_socket):
  state(state),
  fd_socket(fd_socket) {}

CarServer::~CarServer(){
  delete server_thread;
}

std::string CarServer::GetIPAddress() {
  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST];

  if (getifaddrs(&ifaddr) == -1) {
      perror("getifaddrs");
      exit(EXIT_FAILURE);
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      if (ifa->ifa_addr == NULL)
          continue;
      s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if((strcmp(ifa->ifa_name, "wlan0")==0) && (ifa->ifa_addr->sa_family==AF_INET)) {
          if (s != 0) {
              printf("getnameinfo() failed: %s\n", gai_strerror(s));
          }
          printf("\tInterface : <%s>\n",ifa->ifa_name );
          printf("\t  Address : <%s>\n", host);
      }
  }

  return std::string(host);

}


void CarServer::SendIPAddress() {
  std::string ipAddr = GetIPAddress();
  SendMessage(ipAddr);
}

void CarServer::Start(){
  s.sin_family = AF_INET;
  s.sin_port = htons(5001);
  s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  SendIPAddress();

  server_thread = new std::thread(&CarServer::SyncronizeState, this);
  server_thread->detach();
}

void CarServer::SendMessage(std::string message) {
  sendto(fd_socket, message.data(), message.size(), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in));
}


void CarServer::SyncronizeState(){
  struct sockaddr_in s;
  std::string curent_position = "This car is amazing !";
  while(true){
    SendMessage(curent_position);
    sleep(10);
  }
}
