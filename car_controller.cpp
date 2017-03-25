#include "car_controller.h"
#include "car_client.h"
#include "car_server.h"

CarController::CarController() {}

void CarController::Start() {
  State state;
  std::string ip = "0.0.0.0";
  int port = 5000;

  CarClient client(state, make_pair(ip, port));

  client.Connect();
  client.Start();
}
