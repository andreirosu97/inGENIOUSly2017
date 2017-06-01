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
  pinMode(BACK_LIGHT_1, OUTPUT);

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
  if(speed==0)
    digitalWrite(BACK_LIGHT_1, HIGH);
  else
    digitalWrite(BACK_LIGHT_1, LOW);
  softPwmWrite(PWM_1, speed);
  softPwmWrite(PWM_2, speed);
}

void CarMotor::SetSpeedRight(int speed) {
  if(speed==0)
    digitalWrite(BACK_LIGHT_1, HIGH);
  else
    digitalWrite(BACK_LIGHT_1, LOW);
  softPwmWrite(PWM_3, speed);
  softPwmWrite(PWM_4, speed);
}

void CarMotor::SetDirectionLeft(Direction direction) {
    if (direction == FORWARD) {
        digitalWrite(PIN_1_1, HIGH);
        digitalWrite(PIN_2_1, HIGH);
        digitalWrite(PIN_1_2, LOW);
        digitalWrite(PIN_2_2, LOW);
    } else if (direction == BACKWARD) {
        digitalWrite(PIN_1_1, LOW);
        digitalWrite(PIN_2_1, LOW);
        digitalWrite(PIN_1_2, HIGH);
        digitalWrite(PIN_2_2, HIGH);
    } else if (direction == STOP) {
        digitalWrite(PIN_1_1, LOW);
        digitalWrite(PIN_2_1, LOW);
        digitalWrite(PIN_1_2, LOW);
        digitalWrite(PIN_2_2, LOW);
    }
}

void CarMotor::SetDirectionRight(Direction direction) {
    if (direction == FORWARD) {
        digitalWrite(PIN_3_1, HIGH);
        digitalWrite(PIN_4_1, HIGH);
        digitalWrite(PIN_3_2, LOW);
        digitalWrite(PIN_4_2, LOW);
    } else if (direction == BACKWARD) {
        digitalWrite(PIN_3_1, LOW);
        digitalWrite(PIN_4_1, LOW);
        digitalWrite(PIN_3_2, HIGH);
        digitalWrite(PIN_4_2, HIGH);
    } else if (direction == STOP) {
        digitalWrite(PIN_3_1, LOW);
        digitalWrite(PIN_4_1, LOW);
        digitalWrite(PIN_3_2, LOW);
        digitalWrite(PIN_4_2, LOW);
    }
}

void CarMotor::SetDirection(Direction direction) {
    if (direction == FORWARD) {
        digitalWrite(BACK_LIGHT_1, LOW);
        SetDirectionLeft(FORWARD);
        SetDirectionRight(FORWARD);
        //std::cout << "FORWARD\n";
    } else if(direction == BACKWARD) {
        digitalWrite(BACK_LIGHT_1, LOW);
        SetDirectionLeft(BACKWARD);
        SetDirectionRight(BACKWARD);
        //std::cout << "BACKWARD\n";
    } else if(direction == STOP) {
        SetDirectionLeft(STOP);
        SetDirectionRight(STOP);
        std::cout << "STOP\n";
    } else if(direction == LEFT) {
        SetDirectionLeft(BACKWARD);
        SetDirectionRight(FORWARD);
        //std::cout << "LEFT\n";
    } else if (direction == RIGHT) {
        SetDirectionLeft(FORWARD);
        SetDirectionRight(BACKWARD);
        //std::cout << "RIGHT\n";
    }
}


CarMotor::TipCorectie CarMotor::GetCorrectionMode() {
  int valStanga = digitalRead(PIN_FOLLOW_STANGA);
  int valMijloc = digitalRead(PIN_FOLLOW_MIJLOC);
  int valDreapta = digitalRead(PIN_FOLLOW_DREAPTA);

  const int NEGRU = 1;
  const int ALB = 0;

  if (valDreapta == ALB && valStanga == ALB && valMijloc == NEGRU)
    return MIJLOC;
  if (valDreapta == ALB && valStanga == NEGRU)
    return DREAPTA;
  if (valStanga == ALB && valDreapta == NEGRU)
    return STANGA;

  return UNKNOWN;
}

void CarMotor::SyncronizeState() {
    while(thread_on) {
        Direction directie = (Direction)state->get_motor_state().first; 
        int speed = state->get_motor_state().second;
        SetDirection(directie);

        if (directie == FORWARD) {
            TipCorectie correction_mode = GetCorrectionMode();

            int vitezaStanga = speed;
            int vitezaDreapta = speed;
            if (correction_mode == STANGA) {
                vitezaStanga = 1;
                vitezaDreapta = std::min(80, speed);
            } else if (correction_mode == DREAPTA) {
                vitezaStanga = std::min(80, speed);
                vitezaDreapta = 1;
            }
            SetSpeedLeft(vitezaStanga);
            SetSpeedRight(vitezaDreapta);
        } else if (directie == STOP) {
            SetSpeedLeft(0);
            SetSpeedRight(0);
        } else if (directie == BACKWARD) {
            SetSpeedLeft(-speed);
            SetSpeedRight(-speed);
        } else if (directie == LEFT || directie == RIGHT) {
            speed = 100;
            SetSpeedLeft(speed);
            SetSpeedRight(speed);
            if (is_turning) {
                clock_t current_time = clock();
                if ((current_time - turn_time) / CLOCKS_PER_SEC > 0.1) {
                    TipCorectie correction_mode = GetCorrectionMode();
                    if (correction_mode == MIJLOC) {
                        state->update_motor_direction(CarState::FORWARD);
                        is_turning = false;
                    }
                }
            } else {
                turn_time = clock();
                is_turning = true;
            }
        }
  }
}

CarMotor::~CarMotor() {
  thread_on = 0;
  motor_thread->join();

  digitalWrite(BACK_LIGHT_1, LOW);
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
