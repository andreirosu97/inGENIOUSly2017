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

  enum STATE {
    STOPPED = 0x01,
    MOVING_IN = 0x02,
    MOVING_OUT = 0x03,
    WAITING = 0x04
  };


private:
  /* ============== STATE PARAMETERS ============== */


  bool route = false;
  int speed = 0;
  Direction direction;
  int shutdown = 0;//car client sets it
  char car_type = 0xff;


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

  const int nr_i_map = 72;
  unsigned int i_map[72][2] = {
    {0xf0c57a7a, 0x02},
    {0xe0b7807a, 0x03},
    {0xb007797a, 0x11},
    {0xa0ab837c, 0x12},
    {0x8083897c, 0x13},
    {0xb0b0847c, 0x14},
    {0x3068907c, 0x21},
    {0x4096117c, 0x22},
    {0x90a0797a, 0x23},
    {0x50ba7b7a, 0x24},
    {0x10eb817a, 0x33},
    {0x30a1797a, 0x34},
    {0xf028827a, 0x41},
    {0x3032817a, 0x42},
    {0xc0d3827a, 0x43},
    {0xa0eb817a, 0x44},
    {0x801a797a, 0x51},
    {0xb0eb817a, 0x52},
    {0x8057797a, 0x53},
    {0xd06a797a, 0x54},
    {0x202efd73, 0x61},
    {0xa053917c, 0x62},
    {0xc02a8d7c, 0x63},
    {0xf083197c, 0x64},
    {0x507c827c, 0x71},
    {0xb069857c, 0x72},
    {0x106b897c, 0x73},
    {0xc056907c, 0x74},
    {0x90da7a7a, 0x81},
    {0xd0bf7f7a, 0x82},
    {0x9079777a, 0x83},
    {0xf016887c, 0x84},
    {0x30628f7c, 0x91},
    {0x901f827c, 0x92},
    {0x401b8d7c, 0x93},
    {0x805b867c, 0x94},
    {0x4094867c, 0xA1},
    {0x30c58d7c, 0xA2},
    {0x4028887c, 0xA3},
    {0xb0921b7c, 0xA4},
    {0x0091867c, 0xB1},
    {0x70ef877c, 0xB2},
    {0xa0ec8a7c, 0xB3},
    {0x60c48a7c, 0xB4},
    {0x3087857c, 0xC1},
    {0x003e8d7c, 0xC2},
    {0x90a08d7c, 0xD1},
    {0x4064847c, 0xD2},
    {0xe065867c, 0xD3},
    {0xb09a8e7c, 0xD4},
    {0xa046617e, 0xE1},
    {0x505b897c, 0xE2},
    {0x50e3967c, 0xE3},
    {0x1027f179, 0xE4},
    {0xb0080f7c, 0xF1},
    {0x80b6f779, 0xF4},
    {0xe425616f, 0xFF},
    {0x84ed6a6f, 0xFF},
    {0xe6941a7e, 0xFF},
    {0xa434666f, 0xFF},
    {0x247b5f6f, 0xFF},
    {0x84a6636f, 0xFF},
    {0x14f6576f, 0xFF},
    {0xa4ca626f, 0xFF},
    {0x84ed676f, 0xFF},
    {0xd4dd686f, 0xFF},
    {0x44ab6f6f, 0xFF},
    {0xa465656f, 0xFF},
    {0x647f5a6f, 0xFF},
    {0x94dd716f, 0xFF},
    {0x24f06b6f, 0xFF},
    {0xe4fd5e6f, 0xFF}
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
    cars_states[8].second = WAITING;
  }

  ~CarState() {std::cout << "CLOSING STATE!" << std::endl;}

  /* ============== STATE QUERY METHODS ============== */
  bool clear_to_pass(char my_position){// my_position == pozitie in intersectie in care am intrat cu statea STOPED
    int i;
    int mask_58=(15<<4); // mask for the A in 0xAB
    int mask_14= 15; // mask for the B in 0xAB
    for(i = 1; i <= 7; i++ )
      if( (mask_58 & my_position) == (mask_58 & cars_states[i].first)){//Suntem intr-o intersectie cu alta masina
        if( cars_states[i].second == 0x02 )// If the car is MOVING IN we wait more
          return false;
        else if( cars_states[i].second != 0x03 ) // daca masina nu e pe moving out
          if(( (mask_14 & cars_states[i].first) == (mask_14 & my_position)) - 1 ||((mask_14 & cars_states[i].first) == 0x04 &&  (mask_14 & my_position) == 0x01))
            return false;
      }
    return true;
  }


  void get_state(){
    std::cout<<"Speed: " << std::dec << this->speed << std::endl;
    std::cout<<"Direction: " << std::dec << this->direction << std::endl;
    std::cout<<"Shut down: " << std::dec << this->shutdown << std::endl;
  }

  bool is_shutting_down() {
    return this->shutdown;
  }

  char get_car_state() {
    return cars_states[8].second;
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
      if ((float)(current_time - stop_time) / CLOCKS_PER_SEC >= 6.0 && clear_to_pass(cars_states[8].first) ) {
        cars_states[8].second = MOVING_IN;
        std::cout << "MOVING IN" << std::endl;
        this->direction = FORWARD;
        this->speed = 50;
      }
    }
    motor_state = std::make_pair(direction, speed);
    return motor_state;
 }

  void decode(){
    int mask = 15;
    int next_point;
    while (!car_route_decoded.empty()) {
        car_route_decoded.pop();
    }
    while( !car_route.empty() ){
      next_point = ( (int)car_route.front()) & mask;
      car_route.pop();
      if(next_point % 2 == entry_point % 2){
        car_route_decoded.push(FORWARD);
      } else if ( next_point== 0x01 && entry_point== 0x04) {
        car_route_decoded.push(LEFT);
      } else if (next_point== 0x04 && entry_point== 0x01) {
        car_route_decoded.push(RIGHT);
      } else if( next_point > entry_point){
        car_route_decoded.push(LEFT);
      } else if( next_point < entry_point){
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
      std::cout << "Reboot!"<<std::endl;
      unsigned char signature[] = {0xAA, 0xBB, 0xCC, 0xDD, 0x00};
      if (strcmp(mesaj + 1, (char*)signature) == 0) {
        this->direction = STOP;
        this->speed = 0;
        cars_states[8].second = WAITING;
      }
    }

    if (mesaj[0] == 0x03 && get_car_state() == WAITING) {
      int i,j=1,len=0;
      int idMasina=0;
      std::cout << "CarMessage" << std::endl;
      do {

        if(!car_route.empty()) {
          std::queue <char> empty;
          car_route.swap(empty);
        }

        idMasina = (int)mesaj[j++];
        //std::cout<<"IdMasina"<<std::hex<<idMasina<<std::endl;
        car_type=mesaj[j++];
        //std::cout<<"Tip masina:"<<std::hex<<(int)car_type<<std::endl;
        len=(int)mesaj[j];
        //std::cout<<std::dec<<"Lungime:"<<len<<std::endl<<std::endl;
        for(i=1; i<=len; i++) {
          car_route.push(mesaj[i+j]);
        }
        j += len+1;
      } while(idMasina<8 && j<strlen(mesaj));
      entry_point=( (int)car_route.front() )& 15;
      car_route.pop();
      this->speed = 40;
      this->direction = FORWARD;
      cars_states[8].second = MOVING_OUT;
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
     std::cout << "UID necunoscut\n";
    } else if (i_map[poz][1] == 0xff){
        if (last_uid != uid) {
            this->direction = car_route_decoded.front();
            std::cout << "Directia " << this->direction << "\n";
            car_route_decoded.pop();
            last_uid = uid;
        }
    } else if(cars_states[8].first != i_map[poz][1]){
        update_state_rf_found(i_map[poz][1]);
        cars_states[8].first = i_map[poz][1];
        std::cout<<std::hex<<(int)cars_states[8].first<<std::endl;
        if (car_route_decoded.empty()) {
            shut_down();
        }
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
      this->direction = STOP;
      stop_time = clock();
    } else if (cars_states[8].second == MOVING_IN) {
      cars_states[8].second = MOVING_OUT;
      this->speed = 40;
      //std::cout << "MOVING OUT!" << std::endl;
    }
  }

};

#endif
