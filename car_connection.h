#ifndef _CAR_CONNECTION_H_
#define _CAR_CONNECTION_H_

#include "car_state.h"
#include "car_client.h"
#include "car_server.h"

class CarConnection {
  std::pair<const std::string, const int> serverAddress;
  int fd_socket;
  CarState* state;
  std::unique_ptr<CarServer> server;
  std::unique_ptr<CarClient> client;

public:
  CarConnection(CarState* state, std::pair<const std::string, const int> serverAddress);
  ~CarConnection();
  void Connect();
  void Start();
};


#endif
