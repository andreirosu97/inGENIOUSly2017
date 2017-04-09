#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <string>
#include <queue>
#include <mutex>

class CarState {
private:
  int direction=0;
  int speed=0;

  int shutdown=0;//car client sets it

  std::mutex update_state;

public:

  void update_motor_direction(std::string direction) {
    std::lock_guard<std::mutex> guard(update_state);
    if(diretion == "SHUTDOWN"){
        this->diretion=this->speed=0;
        shut_down();
    }
    else if(direction=="FORWARD")
          this->direction=1;
         else if(direction=="BACKWARD")
                this->direction=-1;
              else if(direction=="STOP"){
                    this->direction=0;
                    this->speed=0;
                  }
  }

  void update_motor_speed(int speed):speed(speed){}

  std::pair<int,int> get_motor_state() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);

    std::pair<int,int> motor_state;
    motor_state= std::make_pair(this->direction,this->speed);
    return motor_state;
  }

  void get_state(){
    std::cout<<"Speed:"<<this->speed<<'\n';
    std::cout<<"Direction:"<<this->direction<<'\n';
    std:cout<<"Shut down:"<<this->shutdown<<'\n';
  }

  void shut_down() {
    shutdown = true;
  }

  bool is_shutting_down() {
    return shutdown;
  }
};

#endif
