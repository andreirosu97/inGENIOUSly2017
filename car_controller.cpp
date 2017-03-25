#include "car_controller.h"
#include "car_state.h"
#include "car_connection.h"

CarController::CarController() {}

void CarController::Start() {
  CarState* state = new CarState();
  std::string ip = "0.0.0.0";
  int port = 5000;

  CarConnection* connection = new CarConnection(state, make_pair(ip, port));

  connection->Connect();
  connection->Start();


}
