#include "car_controller.h"
#include "car_client.h"
#include "car_server.h"

void CarController::Start() {
  State state;
  std::string ip = "127.0.0.1";
  int port = 5000;

  CarClient client(state, make_pair(ip, port));

  client.Connect();
  client.Start();
}
