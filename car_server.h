#ifndef _CAR_SERVER_H_
#define _CAR_SERVER_H_

#include <string>
#include "car_state.h"
#include <thread>

class CarServer {

private:
    int fd_socket;
    CarState* state;
    std::thread* ServerThread;

  public:
    CarServer(CarState* state, int socket);
    ~CarServer();
    void Start();

  private:
    void SyncronizeState();
};

#endif
