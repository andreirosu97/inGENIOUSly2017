#include "car_motor.h"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <algorithm>

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

  pinMode(PIN_FOLLOW_STANGA, INPUT);
  pinMode(PIN_FOLLOW_DREAPTA, INPUT);
  pinMode(PIN_FOLLOW_MIJLOC, INPUT);

  softPwmCreate(PWM_1, 0, 100);
  softPwmCreate(PWM_2, 0, 100);
  softPwmCreate(PWM_3, 0, 100);
  softPwmCreate(PWM_4, 0, 100);

  motor_thread = std::unique_ptr<std::thread>(new std::thread(&CarMotor::SyncronizeState, this));
}

CarMotor::CarMotor(CarState* state) {
  this->state = state;
}

void CarMotor::SetSpeedLeft(int speed) {
  softPwmWrite(PWM_1, speed);
  softPwmWrite(PWM_2, speed);
}

void CarMotor::SetSpeedRight(int speed) {
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

CarMotor::TipCorectie CarMotor::GetCorrectionMode() {
  int valStanga = digitalRead(PIN_FOLLOW_STANGA);
  int valMijloc = digitalRead(PIN_FOLLOW_MIJLOC);
  int valDreapta = digitalRead(PIN_FOLLOW_DREAPTA);

  std::cout << "VALOARE STANGA: " << valStanga << "\n";
  std::cout << "VALOARE MIJLOC: " << valMijloc << "\n";
  std::cout << "VALOARE DREAPTA: " << valDreapta << "\n";

  if (valDreapta == 0 && valStanga == 1)
    return DREAPTA;
  if (valStanga == 0 && valDreapta == 1)
    return STANGA;

  return MIJLOC;
}

void CarMotor::SyncronizeState() {
  while(thread_on) {
    if (state->get_car_state() == 1) {  // mersul inainte
      std::pair<int, int> motorState = state->get_motor_state();
      SetDirection(motorState.first);
      TipCorectie correction_mode = GetCorrectionMode();

      int vitezaStanga = motorState.second;
      int vitezaDreapta = motorState.second;
      if (correction_mode == STANGA) {
        vitezaStanga = 1;
        vitezaDreapta = std::min(80, motorState.second * 2);
        std::cout << "Corectie stanga!\n";
      } else if (correction_mode == DREAPTA) {
        vitezaStanga = std::min(80, motorState.second * 2);
        vitezaDreapta = 1;
        std::cout << "Corectie dreapta!\n";
      }

      SetSpeedLeft(vitezaStanga);
      SetSpeedRight(vitezaDreapta);
      //sleep(0.05);
    }
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
