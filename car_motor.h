#ifndef _CAR_MOTOR_H_
#define _CAR_MOTOR_H_

#include "car_state.h"

class CarMotor {
  private:
    CarState* state;
  public:
    CarMotor(CarState* state);
    ~CarMotor();
    void Start();
  private:


};

#endif
