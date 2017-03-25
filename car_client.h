#ifndef _CAR_CLIENT_H_
#define _CAR_CLIENT_H_

#include <string>
#include "state.h"

class CarClient {

  private:
    pair<std::string, int> serverAddress;
    int fd_socket;
    State state;
    ~CarClient();

  public:
    CarClient(State state, pair<std::string, int> serverAddress);
    void Connect();
    void Start();

  private:
    void SyncronizeState();
};

#endif
