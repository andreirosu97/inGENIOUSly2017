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
    std::unique_ptr<std::thread> client_thread;
    int thread_on = 1;

    void SyncronizeState();

  public:
    CarClient(CarState* state, int fd_socket);
    ~CarClient();
    void Start();

  private:

};

#endif
