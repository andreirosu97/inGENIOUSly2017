#ifndef _CAR_RF_H_
#define _CAR_RF_H_

#include <string>
#include "car_state.h"
#include <thread>

class CarRF {
  private:

    union {
      char inputData[20];
      int outputData;
    } buffer_tag, buffer_uid;

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
