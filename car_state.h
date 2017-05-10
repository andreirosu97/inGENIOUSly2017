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
  int direction=1;
  int speed=40;
  int shutdown=0;//car client sets it
  clock_t stop_time;
  std::vector<std::pair<char, char>> cars_states;

  std::mutex update_state;

public:

  enum STATE {
    STOPPED = 1,
    MOVING_IN = 2,
    MOVING_OUT = 3
  };

  unsigned int i_map[8][2] = {
    {0xa0b8557e, 0x11},
    {0xc0fc187c, 0x12},
    {0xc0d9857c, 0x13},
    {0x9dcf92ab, 0x14},
    {0x804bfb79, 0x21},
    {0x70d08a7c, 0x23},
    {0x859e8dab, 0xff},
    {0x8570a8a5, 0xfe}
  };

  const int nr_i_map = 8;

  STATE cur_state = MOVING_OUT; // IN cur_state avem stateul masinii
  unsigned int last_rf_tag = 0x01;

  CarState() {
    cars_states.resize(9);
    cars_states[8].first = 0x01;
    cars_states[8].second = 0x02;
  }

  int get_direction() {
    return direction;
  }

  void update_state_rf_found(unsigned int tag_id) {
    std::cout << tag_id << " " << last_rf_tag << "\n";
    if (cur_state == MOVING_OUT && tag_id != last_rf_tag) {
      cur_state = STOPPED;
      stop_time = clock();
    } else if (cur_state == MOVING_IN && tag_id != last_rf_tag) {
      cur_state = MOVING_OUT;
    }
  }

  void update_rf_tag(unsigned int uid) {
    std::lock_guard<std::mutex> guard(update_state);
    int found = 0, poz;

    for (int iterator = 0; iterator < nr_i_map; ++iterator) {
      if (i_map[iterator][0] == uid) {
        found = 1;
        poz = iterator;
      }
    }

    if (!found) {;
      std::cout << "Eroare RF TAG, tagul " << uid << " nu a fost gasit" << std::endl;
    } else {
      update_state_rf_found(i_map[poz][1]);
      last_rf_tag = i_map[poz][1];
    }
  }

  void get_my_state(unsigned char* state){
    std::lock_guard<std::mutex> guard(update_state);
    state[0] = 0x01;
    state[1] = 0x08;
    state[2] = 0xff;
    state[3] = (char)last_rf_tag;
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
      if ((current_time - stop_time) / CLOCKS_PER_SEC >= 3.0) {
        cur_state = MOVING_IN;
        std::cout<<"MOVING IN"<<std::endl;
        motor_state = std::make_pair(this->direction,this->speed);
      } else {
        //=====================MIGHT BE A bug====================//
        motor_state = std::make_pair(0, 0);
      }
    }
    else {
      motor_state= std::make_pair(this->direction,this->speed);
    }

    return motor_state;
  }

  void get_state(){
    std::cout<<"Speed:"<<this->speed<<std::endl;
    std::cout<<"Direction:"<<this->direction<<std::endl;
    std::cout<<"Shut down:"<<this->shutdown<<std::endl;
  }

  void shut_down() {
    shutdown = true;
  }

  bool is_shutting_down() {
    return shutdown;
  }
};

#endif
