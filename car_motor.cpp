#include "car_motor.h"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>

void CarMotor::Start() {
  if(wiringPiSetup() < 0) {
    std::cout << "Nu a mers wiringPiSetupul";
    state->shut_down();
  }

  pinMode(PWM_1, OUTPUT);
  pinMode(PIN_1_1, OUTPUT);
  pinMode(PIN_1_2, OUTPUT);

  pinMode(PWM_2, OUTPUT);
  pinMode(PIN_2_1, OUTPUT);
  pinMode(PIN_2_2, OUTPUT);

  pinMode(PWM_3, OUTPUT);
  pinMode(PIN_3_1, OUTPUT);
  pinMode(PIN_3_2, OUTPUT);

  pinMode(PWM_4, OUTPUT);
  pinMode(PIN_4_1, OUTPUT);
  pinMode(PIN_4_2, OUTPUT);


  softPwmCreate(PWM_1, 0, 100);
  softPwmCreate(PWM_2, 0, 100);
  softPwmCreate(PWM_3, 0, 100);
  softPwmCreate(PWM_4, 0, 100);

  motor_thread = std::unique_ptr<std::thread>(new std::thread(&CarMotor::SyncronizeState, this));
}

CarMotor::CarMotor(CarState* state) {
  this->state = state;
}

void CarMotor::SetSpeed(int speed) {
  softPwmWrite(PWM_1, speed);
  softPwmWrite(PWM_2, speed);
  softPwmWrite(PWM_3, speed);
  softPwmWrite(PWM_4, speed);
}

void CarMotor::SetDirection(int direction) {
  if (direction == 1) {

    digitalWrite(PIN_1_1, HIGH);
    digitalWrite(PIN_2_1, HIGH);
    digitalWrite(PIN_3_1, HIGH);
    digitalWrite(PIN_4_1, HIGH);
    digitalWrite(PIN_1_2, LOW);
    digitalWrite(PIN_2_2, LOW);
    digitalWrite(PIN_3_2, LOW);
    digitalWrite(PIN_4_2, LOW);

  } else if(direction == -1){

    digitalWrite(PIN_1_1, LOW);
    digitalWrite(PIN_2_1, LOW);
    digitalWrite(PIN_3_1, LOW);
    digitalWrite(PIN_4_1, LOW);
    digitalWrite(PIN_1_2, HIGH);
    digitalWrite(PIN_2_2, HIGH);
    digitalWrite(PIN_3_2, HIGH);
    digitalWrite(PIN_4_2, HIGH);

  }else if(direction == 0){

    digitalWrite(PIN_1_1, LOW);
    digitalWrite(PIN_2_1, LOW);
    digitalWrite(PIN_3_2, LOW);
    digitalWrite(PIN_4_2, LOW);
    digitalWrite(PIN_1_2, LOW);
    digitalWrite(PIN_2_2, LOW);
    digitalWrite(PIN_3_1, LOW);
    digitalWrite(PIN_4_1, LOW);

  }
}

void CarMotor::SyncronizeState() {
  while(thread_on) {
    std::pair<int, int> motorState = state->get_motor_state();
    SetDirection(motorState.first);
    SetSpeed(motorState.second);
    sleep(0.05);
  }
}

CarMotor::~CarMotor() {
  thread_on = 0;
  motor_thread->join();
  softPwmWrite(PWM_1, 0);
  softPwmWrite(PWM_2, 0);
  softPwmWrite(PWM_3, 0);
  softPwmWrite(PWM_4, 0);
  digitalWrite(PIN_1_1, LOW);
  digitalWrite(PIN_2_1, LOW);
  digitalWrite(PIN_3_2, LOW);
  digitalWrite(PIN_4_2, LOW);
  digitalWrite(PIN_1_2, LOW);
  digitalWrite(PIN_2_2, LOW);
  digitalWrite(PIN_3_1, LOW);
  digitalWrite(PIN_4_1, LOW);
  std::cout<< "CLOSING MOTORS!" << std::endl;
}
