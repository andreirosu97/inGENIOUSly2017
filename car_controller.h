#ifndef _CAR_CONTROLLER_H_
#define _CAR_CONTROLLER_H_

#include "car_state.h"
#include "car_connection.h"

class CarController {
  private:
    CarConnection* connection; // OWNER
    CarState* state; // OWNER
  public:
    CarController();
    ~CarController();
    void Start();
  private:


};

#endif
