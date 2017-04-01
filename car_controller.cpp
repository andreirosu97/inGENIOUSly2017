#include "car_controller.h"
#include <unistd.h>
CarController::CarController() {}

void CarController::Start() {
  state = new CarState();
  std::string ip = "0.0.0.0";
  int port = 5000;

  connection = new CarConnection(state, make_pair(ip, port));

  connection->Connect();
  connection->Start();
  while(true)
  {
    if(state->is_shutting_down()){
      break;
    }
    sleep(1);
  }

}

CarController::~CarController() {
  delete state;
  delete connection;
}
