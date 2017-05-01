#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <string>
#include <cstring>
#include <queue>
#include <mutex>
#include <iostream>

class CarState {
private:
  int direction=0;
  int speed=0;

  int shutdown=0;//car client sets it

  std::mutex update_state;

public:
  int get_direction() {
    return direction;
  }

  void update_continental(char* mesaj) {
    if (mesaj[0] == 0x02) {
      unsigned char signature[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00};
      if (strcmp(mesaj + 1, (char*)signature) == 0) {
        this->direction=0;
        this->speed=0;
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

    std::pair<int,int> motor_state;
    motor_state= std::make_pair(this->direction,this->speed);
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
