#ifndef _CAR_SERVER_H_
#define _CAR_SERVER_H_

#include <string>
#include "car_state.h"

class CarServer {

  private:
    std::pair<const std::string, const int> serverAddress;
    int fd_socket;
    CarState* state;

  public:
    CarServer(CarState* state, int socket);
    ~CarServer();
    void Start();

  private:
    void SyncronizeState();
};

#endif
