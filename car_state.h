#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <ctime>
#include <string>
#include <cstring>
#include <vector>
#include <mutex>
#include <iostream>

class CarState {
private:
  int direction=0;
  int speed=0;
  int shutdown=0;//car client sets it
  clock_t stop_time;
  std::vector<std::pair<char, char>> cars_states;

  std::mutex update_state;

public:

  enum STATE {
    STOPPED = 1,
    MOVING_IN = 2,
    MOVING_OUT =  3
  }

  STATE cur_state = MOVING_IN; // IN cur_state avem stateul masinii
  char last_rf_tag = 0x00;

  CarState() {
    cars_states.resize(9);
    cars_states[8].first = 0xA3;
    cars_states[8].second = 0xAD;
  }

  int get_direction() {
    return direction;
  }

  void rf_found() {
    std::lock_guard<std::mutex> guard(update_state);
    if (cur_state == MOVING_OUT) {
      cur_state = STOPPED;
      stop_time = clock();
    } else if (cur_state == MOVING_OUT) {
      cur_state = MOVING_IN;
    }
  }

  void update_rf_tg(char* uid) {
    std::lock_guard<std::mutex> guard(update_state);

    //aici facem maparea dintre codul UID si numarul tagului
    //numarul tagului se pune in last_rf_tag
  }

  void get_my_state(unsigned char* state){
    std::lock_guard<std::mutex> guard(update_state);
    state[0] = 0x01;
    state[1] = 0x08;
    state[2] = 0xff;
    state[3] = last_rf_tag;
    state[4] = cur_state;
    state[5] = 0x00;
  }
  
  void update_continental(char* mesaj) {
    std::lock_guard<std::mutex> guard(update_state);

    if (mesaj[0] == 0x01) {
      int idMasina = (int)mesaj[1];
      cars_states[idMasina] = std::make_pair(mesaj[3], mesaj[4]);
    }

    if (mesaj[0] == 0x02) {
      unsigned char signature[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00};
      if (strcmp(mesaj + 1, (char*)signature) == 0) {
        this->direction=0;
        this->speed=0;
        cars_states[8].second = 0x01;
      }
    }
  }

  void update_motor_direction(std::string direction) {
    std::lock_guard<std::mutex> guard(update_state);
    if(direction == "SD"){
        this->direction=this->speed=0;
        shut_down();
    } else if(direction=="F") {
        this->direction=1;
    } else if(direction=="B") {
        this->direction=-1;
    } else if(direction=="S"){
        this->direction=0;
        this->speed=0;
    }
  }

  void update_motor_speed(int speed){
    this->speed=speed;
  }

  std::pair<int,int> get_motor_state() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);

    std::pair<int, int> motor_state;

    if (cur_state == STOPPED) {
      clock_t current_time = clock();

      if ((stop_time - current_time) / CLOCKS_PER_SEC >= 3.0) {
        cur_state = MOVING_IN;
        motor_state = std::make_pair(this->direction,this->speed);
      } else {
        motor_state = std::make_pair(0, 0);
      }
    }
    else {
      motor_state= std::make_pair(this->direction,this->speed);
    }

    return motor_state;
  }

  void get_state(){
    std::cout<<"Speed:"<<this->speed<<'\n';
    std::cout<<"Direction:"<<this->direction<<'\n';
    std::cout<<"Shut down:"<<this->shutdown<<'\n';
  }

  void shut_down() {
    shutdown = true;
  }

  bool is_shutting_down() {
    return shutdown;
  }
};

#endif
