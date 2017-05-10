#ifndef _CAR_MOTOR_H_
#define _CAR_MOTOR_H_

#include "car_state.h"
#include <thread>

//Mottors LEFT
#define PWM_1 27
#define PIN_1_1 28
#define PIN_1_2 29

#define PWM_2 23
#define PIN_2_1 24
#define PIN_2_2 25

//Motors RIGHT
#define PWM_3 26
#define PIN_3_1 21
#define PIN_3_2 22

#define PWM_4 3
#define PIN_4_1 4
#define PIN_4_2 5

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
