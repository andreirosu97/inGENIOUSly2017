#ifndef _CAR_SERVER_H_
#define _CAR_SERVER_H_

#include "car_state.h"
#include <string>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <thread>

class CarServer {

private:
    int fd_socket;
    CarState* state;
    std::unique_ptr<std::thread> server_thread;
    struct sockaddr_in s;
    int thread_on = 1;
    int port;
    
    void SendMessage(char* message);
    void SyncronizeState();

  public:
    CarServer(CarState* state, int fd_socket, int port);
    ~CarServer();
    void Start();

};

#endif
