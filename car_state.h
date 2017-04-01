#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <string>

class CarState {
private:
  int state;
  int shut_down=0;//car client sets it
  queue<std::string> message;
  std::mutex update_state;

public:
  void update_message(std::string message) {
    std::lock_guard<std::mutex> guard(update_state);
    this->message.push(message);
  }
  bool is_message() {
    return message.size() > 0;
  }
  std::string get_message() { // daca nu exista mesaj?
    std::lock_guard<std::mutex> guard(update_state);
    std::string current_message = message.front();
    message.pop();
    return current_message;
  }
  
  void shut_down() {
    show_down = true;
  }
};

#endif
