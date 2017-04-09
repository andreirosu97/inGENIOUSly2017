#include "car_motor.h"
#include <iostream>
#include <thread>
#include <wiringPi.h>
#include <softPwm.h>

void Start() {
  if(wiringPiSetup() < 0) {
    std::cout << "Nu a mers wiringPiSetupul";
    state->shut_down();
  }
  pinMode(PWM_PIN, OUTPUT);
  pinMode(PIN_1, OUTPUT);
  pinMode(PIN_2, OUTPUT);

  softPwmCreate(PWM_PIN, 0, 100);

  motor_thread = new std::thread(&CarMotor::SyncronizeState, this);
  motor_thread->detach();
}

CarMotor::CarMotor(CarState* state) {
  this->state = state;
}

void CarMotor::SetSpeed(int speed) {
  softPwmWrite(PWM_PIN, speed);
}

void CarMotor::SetDirection(int direction) {
  if (direction) {
    digitalWrite(PIN_1, HIGH);
    digitalWrite(PIN_2, LOW);
  } else {
    digitalWrite(PIN_1, LOW);
    digitalWrite(PIN_2, HIGH);
  }
}

void CarMotor::SyncronizeState() {
  while(true) {
    std::pair<int, int> motorState = state->get_motor_state();
    SetDirection(motorState.first);
    SetSpeed(motorState.second);
    sleep(0.05);
  }
}

CarMotor::~CarMotor() {
  delete motor_thread;
}
