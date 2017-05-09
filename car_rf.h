#ifndef _CAR_RF_H_
#define _CAR_RF_H_

#include <string>
#include "car_state.h"
#include <thread>

class CarRF {
  private:
    char bufferTag[20], bufferUID[20];
    CarState* state;
    std::thread* rf_thread;
    void SyncronizeState();

  public:
    CarRF(CarState* state);
    ~CarRF();
    void Start();

  private:

};

#endif
