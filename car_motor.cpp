#include "car_motor.h"
#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <algorithm>

void CarMotor::Start() {
  if(wiringPiSetup() < 0) {
    std::cout << "Nu a mers wiringPiSetupul"<<std::endl;
    state->shut_down();
  }
  pinMode(BACK_LIGHT, OUTPUT);
  pinMode(RIGHT_YELLOW_LIGHT, OUTPUT);
  pinMode(LEFT_YELLOW_LIGHT, OUTPUT);

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
    digitalWrite(BACK_LIGHT, HIGH);
  else
    digitalWrite(BACK_LIGHT, LOW);
  softPwmWrite(PWM_1, speed);
  softPwmWrite(PWM_2, speed);
}

void CarMotor::SetSpeedRight(int speed) {
  if(speed==0)
    digitalWrite(BACK_LIGHT, HIGH);
  else
    digitalWrite(BACK_LIGHT, LOW);
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
        digitalWrite(BACK_LIGHT, LOW);
        SetDirectionLeft(FORWARD);
        SetDirectionRight(FORWARD);
        //std::cout << "FORWARD\n";
    } else if(direction == BACKWARD) {
        digitalWrite(BACK_LIGHT, LOW);
        SetDirectionLeft(BACKWARD);
        SetDirectionRight(BACKWARD);
        //std::cout << "BACKWARD\n";
    } else if(direction == STOP) {
        digitalWrite(BACK_LIGHT, HIGH);
        SetDirectionLeft(STOP);
        SetDirectionRight(STOP);
        //std::cout << "STOP\n";
    } else if(direction == LEFT) {
        digitalWrite(BACK_LIGHT, LOW);
        SetDirectionLeft(BACKWARD);
        SetDirectionRight(FORWARD);
        //std::cout << "LEFT\n";
    } else if (direction == RIGHT) {
        digitalWrite(BACK_LIGHT, LOW);
        SetDirectionLeft(FORWARD);
        SetDirectionRight(BACKWARD);
        //std::cout << "RIGHT\n";
    }
}


CarMotor::TipCorectie CarMotor::GetCorrectionMode() {
  int valStanga = digitalRead(PIN_FOLLOW_STANGA);
  int valMijloc = digitalRead(PIN_FOLLOW_MIJLOC);
  int valDreapta = digitalRead(PIN_FOLLOW_DREAPTA);
  std::cout<<valStanga<<" "<<valMijloc<<" "<<valDreapta<<std::endl;
  const int NEGRU = 1;
  const int ALB = 0;

  if (valDreapta == ALB && valStanga == ALB && valMijloc == NEGRU)
    return MIJLOC;
  if ( valDreapta == ALB && valStanga == NEGRU)
    return DREAPTA;
  if ( valStanga == ALB && valDreapta == NEGRU)
    return STANGA;
  return UNKNOWN;
}

void CarMotor::BlinkIfNecessary(clock_t current_time, Direction directie) {
    clock_t diff = (current_time - last_blink) / CLOCKS_PER_SEC;
    if(directie == LEFT){
      digitalWrite(RIGHT_YELLOW_LIGHT, LOW);
      if(diff > 0.3){
          digitalWrite(LEFT_YELLOW_LIGHT, !blink_led_on);
          last_blink = current_time;
          blink_led_on = !blink_led_on;
        }
      }
    else{
      digitalWrite(LEFT_YELLOW_LIGHT, LOW);
      if(diff > 0.3){
          digitalWrite(RIGHT_YELLOW_LIGHT, !blink_led_on);
          last_blink = current_time;
          blink_led_on = !blink_led_on;
        }
    }
}

void CarMotor::ResetBlinks() {
    blink_led_on = false;
    digitalWrite(LEFT_YELLOW_LIGHT, blink_led_on);
    digitalWrite(RIGHT_YELLOW_LIGHT, blink_led_on);
}

void CarMotor::SyncronizeState() {
    while(thread_on) {
        std::pair<int, int> m_state = state->get_motor_state();
        Direction directie = (Direction)m_state.first;
        int speed = m_state.second;
        SetDirection(directie);

        if (directie == FORWARD) {
            TipCorectie correction_mode = GetCorrectionMode();

            int vitezaStanga = speed;
            int vitezaDreapta = speed;
            if (correction_mode == STANGA) {
                vitezaStanga = 100;
                vitezaDreapta = 1;
            } else if (correction_mode == DREAPTA) {
                vitezaStanga = 1;
                vitezaDreapta = 100;
            }
            SetSpeedLeft(vitezaStanga);
            SetSpeedRight(vitezaDreapta);
        } else if (directie == STOP) {
            SetSpeedLeft(0);
            SetSpeedRight(0);
        } else if (directie == BACKWARD) {
            SetSpeedLeft(speed);
            SetSpeedRight(speed);
        } else if (directie == LEFT || directie == RIGHT) {
            SetSpeedLeft(speed);
            SetSpeedRight(speed);
            if (is_turning) {
                clock_t current_time = clock();
                BlinkIfNecessary(current_time, directie);
                if ((float)(current_time - turn_time) / CLOCKS_PER_SEC > 1) {
                    //std::cout<<"Started checking"<<std::endl;
                    TipCorectie correction_mode = GetCorrectionMode();
                    if (correction_mode != UNKNOWN) {
                        state->update_motor_direction(CarState::FORWARD);
                        is_turning = false;
                        ResetBlinks();
                    }
                }
            } else {
                turn_time = clock();
                last_blink = turn_time;
                is_turning = true;
            }
        }
  }
}

CarMotor::~CarMotor() {
  thread_on = 0;
  motor_thread->join();

  digitalWrite(BACK_LIGHT, LOW);
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
