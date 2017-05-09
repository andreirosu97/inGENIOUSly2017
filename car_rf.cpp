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

CarRF::~CarRF() {
  delete rf_thread;
}

void CarRF::Start() {
  wiringPiSPISetup(0, 10000000);
  InitRc522();
  rf_thread = new std::thread(&CarRF::SyncronizeState, this);
  rf_thread->detach();
}

void CarRF::SyncronizeState() {
  while(true){
      unsigned char status = PcdRequest(PICC_REQIDL, buffer_tag);
      if(status == TAG_OK){
          PcdAnticoll(0, buffer_uid);
          unsigned int uid = (buffer_uid[0]<<24)|(buffer_uid[1]<<16)|(buffer_uid[2]<<8)|buffer_uid[3];
          std::cout << std::hex << "UIDul este " << uid << "\n";
          state->update_rf_tag(uid);
      }
    }
}
