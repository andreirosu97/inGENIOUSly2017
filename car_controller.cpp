#include "car_controller.h"
#include "car_client.h"
#include "car_server.h"

void CarController::Start() {
  CarClient client;
  
  client.connect();
  client.start();



}
