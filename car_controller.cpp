#include "car_controller.h"

CarController::CarController() {}

void CarController::Start() {
  state = new CarState();
  std::string ip = "0.0.0.0";
  int port = 5000;

  connection = new CarConnection(state, make_pair(ip, port));

  connection->Connect();
  connection->Start();


}


CarController::~CarController() {
  delete state;
  delete connection;
}
