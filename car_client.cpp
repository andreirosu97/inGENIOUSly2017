#include "car_client.h"

CarClient::CarClient(State state, pair<std::string, int> serverAddress :
  state{state},
  serverAddress{serverAddress}
  {}
