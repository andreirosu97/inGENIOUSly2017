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
  void update_motor_state(std::string direction, int speed) {
    std::lock_guard<std::mutex> guard(update_state);

    this->speed=speed;
    if(direction=="FORWARD")
      this->direction=1;
    else if(direction=="BACKWARD")
        this->direction=-1;
        else if(direction=="STOP"){
          this->direction=0;
          this->speed=0;
          }
  }

  std::pair get_motor_state() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);

    std::pair<int,int> motor_state;
    motor_spate= std::make_pair(this->direction,this->speed);
    return motor_state;
  }

  void shut_down() {
    shutdown = true;
  }

  bool is_shutting_down() {
    return shutdown;
  }
};

#endif
