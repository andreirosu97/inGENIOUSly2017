#ifndef _CAR_CONNECTION_H_
#define _CAR_CONNECTION_H_

#include "car_state.h"

class CarConnection {
  std::pair<const std::string, const int> serverAddress;
  int fd_socket;
  CarState* state;

public:
  CarConnection(CarState* state, std::pair<const std::string, const int> serverAddress);
  ~CarConnection();
  void Connect();
  void Start();

};


#endif
