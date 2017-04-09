#ifndef _CAR_MOTOR_H_
#define _CAR_MOTOR_H_

#include "car_state.h"
#include <thread>

#define PWM_PIN 0
#define PIN_1 1
#define PIN_2 2

class CarMotor {
  private:
    CarState* state;
    std::thread* motor_thread;

    void SyncronizeState();
    void SetDirection (int direction);
    void SetSpeed (int speed);

  public:
    CarMotor(CarState* state);
    ~CarMotor();
    void Start();

};

#endif
