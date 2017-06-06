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
public:
  enum Direction {
    FORWARD = 0,
    BACKWARD = 1,
    LEFT = 2,
    RIGHT = 3,
    STOP = 4
  };

private:
  /* ============== STATE PARAMETERS ============== */


  bool route = false;
  int speed = 0;
  Direction direction;
  int shutdown = 0;//car client sets it
  char car_type = 0xff;

  enum STATE {
    STOPPED = 0x01,
    MOVING_IN = 0x02,
    MOVING_OUT = 0x03
  };

  clock_t stop_time;
  std::vector<std::pair<char, char>> cars_states;
  std::queue <char> car_route;
  std::queue <Direction> car_route_decoded;
  unsigned int last_uid;

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
    int i;
    for(i = 1; i <= 7; i++){
      cars_states[i].first = 0x01;
      cars_states[i].second = 0x01;
    }
    cars_states[8].first = 0x01;
    cars_states[8].second = 0x03;
  }

  ~CarState() {std::cout << "CLOSING STATE!" << std::endl;}

  /* ============== STATE QUERY METHODS ============== */
  bool clear_to_pass(char my_position){// my_position == pozitie in intersectie in care am intrat cu statea STOPED
    int i;
    int mask_58=(15<<4); // mask for the A in 0xAB
    int mask_14= 15; // mask for the B in 0xAB
    for(i = 1; i <= 7; i++ )
      if( mask_58 & my_position == mask_58 & cars_states[i].first){//Suntem intr-o intersectie cu alta masina
        if( cars_states[i].second == 0x02 )// If the car is MOVING IN we wait more
          return false;
        else if( mask_14 & cars_states[i].first < mask_14 & my_position || ( mask_14 & cars_states[i].first == 0x04 &&  mask_14 & my_position == 0x01))
          return false;
      }
    return true;
  }


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

  std::pair<int, int> get_motor_state() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);

    std::pair<Direction, int> motor_state;

    if (cars_states[8].second == STOPPED) {
      clock_t current_time = clock();
      if ((current_time - stop_time) / CLOCKS_PER_SEC >= 3.0 ){//&& clear_to_pass(cars_states[8].first) ) {
        cars_states[8].second = MOVING_IN;
        //std::cout << "MOVING IN" << std::endl;
        this->speed = 100;
        motor_state = std::make_pair(FORWARD, speed);
      } else{
        motor_state = std::make_pair(STOP, speed);
      }
    } else {
      motor_state = std::make_pair(direction, speed);
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
        car_route_decoded.push(FORWARD);
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
        this->direction = STOP;
        this->speed = 0;
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
        //std::cout<<"IdMasina"<<std::hex<<idMasina<<std::endl;
        car_type=mesaj[j++];
        //std::cout<<"Tip masina:"<<std::hex<<(int)car_type<<std::endl;
        len=(int)mesaj[j];
        //std::cout<<std::dec<<"Lungime:"<<len<<std::endl<<std::endl;
        for(i=1; i<=len; i++){
          car_route.push(mesaj[i+j]);
        }
        j+=len+1;
      }while(idMasina<8 && j<strlen(mesaj));
      entry_point=( (int)car_route.front() )& 15;
      car_route.pop();
      this->speed = 60;
      this->direction = FORWARD;
      route=true;
      decode();
    }
  }

  void update_motor_direction(std::string direction) {
    std::lock_guard<std::mutex> guard(update_state);
    if(direction == "SD"){
        this->direction = STOP;
        shut_down();
    } else if(direction=="F") {
        this->direction = FORWARD;
    } else if(direction=="B") {
        this->direction = BACKWARD;
    } else if(direction=="S"){
        this->direction = STOP;
        this->speed = 0;
    }

  }

  void update_motor_speed(int speed){
      this->speed = speed;
  }

  void update_motor_direction(Direction direction) {
      this->direction = direction;
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

    if (!found) {
      if (last_uid != uid) {
         //std::cout << "UID necunoscut, probabil mijloc de intersectie!\n";
         this->direction = car_route_decoded.front();
         //std::cout << "Directia " << this->direction << "\n";
         car_route_decoded.pop();
         last_uid = uid;
      }
    } else if(cars_states[8].first != i_map[poz][1]){
      update_state_rf_found(i_map[poz][1]);
      cars_states[8].first = i_map[poz][1];
    }
  }

  void update_state_rf_found(unsigned int tag_id) {
    /*if( (tag_id == 0x21 || tag_id == 0x13 || tag_id== 0x14 || tag_id==0x12) && cars_states[8].first!=0x01  && cars_states[8].first!=tag_id){
      sleep(2);
      shut_down();
    }*/

    if (cars_states[8].second == MOVING_OUT) {
      cars_states[8].second = STOPPED;
      //std::cout<<"STOPPED"<<std::endl;
      this->speed=0;
      stop_time = clock();
    } else if (cars_states[8].second == MOVING_IN) {
      cars_states[8].second = MOVING_OUT;
      this->speed = 60;
      //std::cout << "MOVING OUT!" << std::endl;
    }
  }

};

#endif
