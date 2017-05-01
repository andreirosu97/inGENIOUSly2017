#ifndef _CAR_MOTOR_H_
#define _CAR_MOTOR_H_

#include "car_state.h"
#include <thread>

#define PWM_1 0
#define PIN_1_1 1
#define PIN_1_2 2

#define PWM_2 3
#define PIN_2_1 4
#define PIN_2_2 5

#define PWM_3 8
#define PIN_3_1 9
#define PIN_3_2 7

#define PWM_4 6
#define PIN_4_1 10
#define PIN_4_2 11

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
