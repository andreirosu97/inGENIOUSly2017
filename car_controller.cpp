#include "car_controller.h"
#include <unistd.h>
CarController::CarController() {}

void CarController::Start() {
  std::string ip = "0.0.0.0";
  int port = 5000;

  state = std::make_unique<CarState>();
  connection = std::make_unique<CarConnection>(state.get(), make_pair(ip, port));
  motor = std::make_unique<CarMotor>(state.get());

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
}
