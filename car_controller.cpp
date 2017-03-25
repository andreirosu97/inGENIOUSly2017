#include "car_controller.h"
#include "car_client.h"
#include "car_server.h"
#include "car_state.h"

CarController::CarController() {}

void CarController::Start() {
  CarState* state = new CarState();
  std::string ip = "0.0.0.0";
  int port = 5000;

  CarClient* client = new CarClient(state, make_pair(ip, port));

  client->Connect();
  client->Start();
}
