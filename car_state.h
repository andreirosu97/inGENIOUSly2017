#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <string>

class CarState {
public:
  int state;
  int new_message=0;
  int shut_down=0;//car client sets it
  std::string message;

};

#endif
