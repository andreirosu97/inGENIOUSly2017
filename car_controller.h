#ifndef _CAR_CONTROLLER_H_
#define _CAR_CONTROLLER_H_

#include "car_state.h"
#include "car_connection.h"
#include "car_motor.h"
#include <iostream>

class CarController {
  private:
    std::unique_ptr<CarConnection> connection;
    std::unique_ptr<CarState> state;
    std::unique_ptr<CarMotor> motor;

  public:
    CarController();
    ~CarController() {std::cout<<"CAR IS SHUTTING DOWN!\n";};
    void Start();
  private:


};

#endif
