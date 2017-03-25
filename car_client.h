#ifndef _CAR_CLIENT_H_
#define _CAR_CLIENT_H_

#include <string>
#include "state.h"

class CarClient {

  private:
    std::pair<const std::string, const int> serverAddress;
    int fd_socket;
    State state;


  public:
    CarClient(State state, std::pair<const std::string, const int> serverAddress);
    ~CarClient();
    void Connect();
    void Start();

  private:
    void SyncronizeState();
};

#endif
