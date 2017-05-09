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
      unsigned char status = PcdRequest(PICC_REQIDL, bufferTag.inputData);
      if(status == TAG_OK){
          PcdAnticoll(0, buffer_uid.inputData);
          state->update_rf_tag(buffer_uid.outputData);
      }
    }
}