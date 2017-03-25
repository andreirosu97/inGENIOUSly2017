#ifndef _CAR_CLIENT_H_
#define _CAR_CLIENT_H_

#include <string>
#include "state.h"

class CarClient {

private:
  pair<std::string, int> serverData;
  int fd_socket;
  State state;

public:

  CarClient(State state, pair<std::string, int> serverData);
  void Connect();
  void Start();

private:
  void SyncronizeState();
}

#endif
