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
      unsigned char status = PcdRequest(PICC_REQIDL, buffer_tag.inputData);
      if(status == TAG_OK){
          PcdAnticoll(0, buffer_uid.inputData);
          buffer_uid.outputData=(buffer_tag.inputData[0]<<24)|(buffer_tag.inputData[1]<<16)|(buffer_tag.inputData[2]<<8)|buffer_tag.inputData[3];
          state->update_rf_tag(buffer_uid.inputData);
      }
    }
}
