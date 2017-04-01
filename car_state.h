#ifndef _CAR_STATE_H_
#define _CAR_STATE_H_
#include <string>
#include <queue>
#include <mutex>

class CarState {
private:
  int state;
<<<<<<< HEAD
  int new_message=0;
  int shut_down=0;//car client sets it
  std::string message;
=======
  int shutdown=0;//car client sets it
  std::queue<std::string> message;
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
>>>>>>> 64bef079d66c45f9a0d2b5bfd2b22f62729bcc2c

  void shut_down() {
    shutdown = true;
  }
  bool is_shutting_down() {
    return shutdown;
  }
};

#endif
