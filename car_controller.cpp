#include "car_controller.h"
#include <unistd.h>

CarController::CarController() {}

void CarController::Start() {
  std::string ip = "0.0.0.0";
  int port = 5001;


  state = std::unique_ptr<CarState>(new CarState());
  motor = std::unique_ptr<CarMotor>(new CarMotor(state.get()));
  connection = std::unique_ptr<CarConnection>(new CarConnection(state.get(), make_pair(ip, port)));
  rf = std::unique_ptr<CarRF>(new CarRF(state.get()));

  motor->Start();
  connection->Connect();
  connection->Start();
  rf->Start();

  while(true) {
    if(state->is_shutting_down()){
      break;
    }
    sleep(1);
  }

}
