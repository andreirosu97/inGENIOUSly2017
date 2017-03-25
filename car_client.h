#ifndef _CAR_CLIENT_H_
#define _CAR_CLIENT_H_

#include <string>
#include "state.h"

class CarClient {

private:
  std::pair<std::string, int> serverAddress;
  int fd_socket;
  State state;

public:

  CarClient(State state, std::pair<std::string, int> serverAddress);
  void Connect();
  void Start();

private:
  void SyncronizeState();
};

#endif
