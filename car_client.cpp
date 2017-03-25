#include "car_client.h"

CarClient::CarClient(State state, std::pair<std::string, int> serverAddress) :
  state(state),
  serverAddress(serverAddress)
  {}
