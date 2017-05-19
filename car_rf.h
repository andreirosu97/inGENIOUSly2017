#ifndef _CAR_RF_H_
#define _CAR_RF_H_

#include <string>
#include "car_state.h"
#include <thread>

class CarRF {
  private:
    unsigned char buffer_tag[20], buffer_uid[20];

    CarState* state;
    std::unique_ptr<std::thread> rf_thread;
    int thread_on=1;
    void SyncronizeState();

  public:
    CarRF(CarState* state);
    ~CarRF();
    void Start();

  private:

};

#endif
