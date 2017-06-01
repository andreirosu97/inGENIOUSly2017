#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <ctime>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>

class CarState {
private:
  /* ============== STATE PARAMETERS ============== */
  bool route = false;
  int direction = 0;
  int speed = 0;
  int shutdown = 0;//car client sets it
  char car_type = 0xff;

  enum STATE {
    STOPPED = 0x01,
    MOVING_IN = 0x02,
    MOVING_OUT = 0x03
  };

  enum TURN{
    NONE = 0,
    RIGHT = 1,
    LEFT = -1
  };

  clock_t stop_time;
  std::vector<std::pair<char, char>> cars_states;
  std::queue <char> car_route;
  std::queue <int> car_route_decoded;

  /* ============== MUTEXES ============== */
  std::mutex update_state;

  /* ============== RFID MAP ============== */

  unsigned int last_rf_tag = 0x01;
  unsigned int entry_point = 0x01;

  const int nr_i_map = 17;
  unsigned int i_map[17][2] = {
    {0xa0b8557e, 0x11},
    {0xc0fc187c, 0x12},
    {0xc0d9857c, 0x13},
    {0x9dcf92ab, 0x14},
    {0x804bfb79, 0x21},
    {0xb0f4157c, 0x22},
    {0x70d08a7c, 0x23},
    {0xb0de0d7c, 0x24},
    {0xe0d9817c, 0x31},
    {0x00f28d7c, 0x32},
    {0x7012897c, 0x33},
    {0x2066fb79, 0x41},
    {0x2057187c, 0x42},
    {0x2035887c, 0x43},
    {0xa0388d7c, 0x54},
    {0xc0ce1b7c, 0x64},
    {0x9cf0bf88, 0xfd}//Card personal
  };


public:

  /* ============== Constructor and Destructor ============== */
  CarState() {
    cars_states.resize(9);
    cars_states[8].first = 0x01;
    cars_states[8].second = 0x03;
  }

  ~CarState() {std::cout << "CLOSING STATE!" << std::endl;}

  /* ============== STATE QUERY METHODS ============== */

  void get_state(){
    std::cout<<"Speed: " << std::dec << this->speed << std::endl;
    std::cout<<"Direction: " << std::dec << this->direction << std::endl;
    std::cout<<"Shut down: " << std::dec << this->shutdown << std::endl;
  }

  bool start_car(){
    return route;
  }

  bool is_shutting_down() {
    return this->shutdown;
  }

  void shut_down() {
    shutdown = true;
  }

  void get_my_state(unsigned char* state){
    std::lock_guard<std::mutex> guard(update_state);
    state[0] = 0x01;
    state[1] = 0x08;
    state[2] = car_type;
    state[3] = cars_states[8].first;
    state[4] = cars_states[8].second;
    state[5] = 0x00;
  }

  std::pair<int,int> get_motor_state() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);

    std::pair<int, int> motor_state;

    if (cars_states[8].second == STOPPED) {
      clock_t current_time = clock();
      if ((current_time - stop_time) / CLOCKS_PER_SEC >= 3.0) {
        cars_states[8].second = MOVING_IN;
        std::cout<<"MOVING IN"<<std::endl;
        motor_state.first=direction;
        motor_state.second=speed;
      } else {
        motor_state = std::make_pair(0, 0);
      }
    }
    else {
      motor_state.first=direction;
      motor_state.second=speed;
    }
    return motor_state;
  }


  void decode(){
    int mask = 15;
    int next_point;
    while( !car_route.empty() ){
      next_point = ( (int)car_route.front()) & mask;
      car_route.pop();
      if( next_point%2 == entry_point%2){
        car_route_decoded.push(NONE);
      }else if( next_point > entry_point || ( next_point== 0x01 && entry_point== 0x04) ){
        car_route_decoded.push(LEFT);
      }
      else if( next_point < entry_point || ( next_point== 0x04 && entry_point== 0x01) ){
        car_route_decoded.push(RIGHT);
      }

      if( next_point == 0x02 )
        entry_point = 0x04;
      else if( next_point == 0x04 )
        entry_point = 0x02;
      else if( next_point == 0x03)
        entry_point = 0x01;
      else
        entry_point = 0x03;
    }
    while(!(car_route_decoded.empty()) ){
      std::cout<<car_route_decoded.front()<<std::endl;
      car_route_decoded.pop();
    }
  }
/* ============= UPDATE STATES METHODS ============== */

  void update_continental(char* mesaj) {
    std::lock_guard<std::mutex> guard(update_state);

    if (mesaj[0] == 0x01) {
      int idMasina = (int)mesaj[1];
      car_type = mesaj[2];
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

    if (mesaj[0] == 0x03 && !route) {
      int i,j=1,len=0;
      int idMasina=0;
      do{

        if(!car_route.empty()){
          std::queue <char> empty;
          car_route.swap(empty);
        }

        idMasina = (int)mesaj[j++];
        std::cout<<"IdMasina"<<std::hex<<idMasina<<std::endl;
        car_type=mesaj[j++];
        std::cout<<"Tip masina:"<<std::hex<<(int)car_type<<std::endl;
        len=(int)mesaj[j];
        std::cout<<std::dec<<"Lungime:"<<len<<std::endl<<std::endl;
        for(i=1; i<=len; i++){
          car_route.push(mesaj[i+j]);
        }
        j+=len+1;
      }while(idMasina<8 && j<strlen(mesaj));
/*
      std::cout<<"The Route is :"<<std::endl;
      while(!car_route.empty()){
        std::cout<<std::hex<<(int)car_route.front()<<std::endl;
        car_route.pop();
      }*/
      entry_point=( (int)car_route.front() )& 15;
      car_route.pop();
      this->speed=40;
      this->direction=1;
      route=true;
      decode();
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

  /* ================= RF UPDATE METHODS ============== */

  void update_rf_tag(unsigned int uid) {
    std::lock_guard<std::mutex> guard(update_state);
    int found = 0, poz;

    //Search
    for (int i = 0; i < nr_i_map; ++i) {
      if (i_map[i][0] == uid) {
        found = 1;
        poz = i;
      }
    }

    if (!found) {;
      std::cout << "Eroare RF TAG, tagul " << uid << " nu a fost gasit" << std::endl;
    } else {
      update_state_rf_found(i_map[poz][1]);
      cars_states[8].first = i_map[poz][1];
    }
  }

  void update_state_rf_found(unsigned int tag_id) {
    /*if( (tag_id == 0x21 || tag_id == 0x13 || tag_id== 0x14 || tag_id==0x12) && cars_states[8].first!=0x01  && cars_states[8].first!=tag_id){
      sleep(2);
      shut_down();
    }*/

    if (cars_states[8].second == MOVING_OUT && tag_id != cars_states[8].first) {
      cars_states[8].second=STOPPED;
      std::cout<<"STOPPED"<<std::endl;
      stop_time = clock();
    } else if (cars_states[8].second == MOVING_IN && tag_id != cars_states[8].first) {
      cars_states[8].second = MOVING_OUT;
      std::cout<<"MOVING OUT!"<<std::endl;
    }
  }

  int get_car_state() {
    return 1;
  }

};

#endif
