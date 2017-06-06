#include "car_controller.h"
#include <unistd.h>
#include <csignal>

namespace {
  int signal_shutdown = 0;
  void SignalHandler(int signal) {
    if (signal == SIGINT)
      signal_shutdown = 1;
  }
}

void CarController::Start() {
  std::string ip = "0.0.0.0";
  int port = 5001;

  std::cout << "Initializarea pornita" << std::endl;

  std::cout << "IP: " << ip << std::endl;;
  std::cout << "Port: " << port << std::endl;;

  state = std::unique_ptr<CarState>(new CarState());
  connection = std::unique_ptr<CarConnection>(new CarConnection(state.get(), make_pair(ip, port)));
  motor = std::unique_ptr<CarMotor>(new CarMotor(state.get()));
  rf = std::unique_ptr<CarRF>(new CarRF(state.get()));

  std::signal(SIGINT, SignalHandler);

  connection->Connect();
  connection->Start();

  while(!state->start_car() && !signal_shutdown);//Sleep till the map was recieved

  std::cout<<"Car Start moving!"<<std::endl;

  motor->Start();
  rf->Start();

  while(true) {
    if (signal_shutdown) {
      state->shut_down();
    }

    if(state->is_shutting_down()) {
      break;
    }

    //state->get_state();
  }
}
