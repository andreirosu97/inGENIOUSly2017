#ifndef _CAR_CLIENT_H_
#define _CAR_CLIENT_H_

#include <string>
#include "car_state.h"
#include <thread>

class CarClient {

  private:
    std::pair<const std::string, const int> serverAddress;
    int fd_socket;
    CarState* state;
    std::thread* ClientThread;

  public:
    CarClient(CarState* state, int fd_socket);
    ~CarClient();
    void Connect();
    void Start();

  private:
    void SyncronizeState();
};

#endif
