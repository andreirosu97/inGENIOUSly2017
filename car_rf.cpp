#include "rc522.h"
#include "car_rf.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <wiringPiSPI.h>
#include <unistd.h>

CarRF::CarRF(CarState *state) {
  this->state = state;
}

void CarRF::Start() {
  wiringPiSPISetup(0, 10000000);
  InitRc522();
  rf_thread = new std::thread(&CarRF::SyncronizeState, this);
  rf_thread->detach();
}

void CarRF::SyncronizeState() {
  while(true){
      unsigned char status = PcdRequest(PICC_REQIDL, bufferTag);
      if(status == TAG_OK){
          state->rf_found();
          PcdAnticoll(0, bufferUID);
          state->update_rf_tg(bufferUID);
      }
    }
}
