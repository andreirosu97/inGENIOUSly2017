#include "rc522.h"
#include "bcm2835.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <wiringPiSPI.h>
#include <unistd.h>
int continue_reading = 1 ;

#define DEFAULT_SPI_SPEED 5000L

int main(){
    wiringPiSPISetup(0, 10000000);
    InitRc522();
    unsigned char *pTagType;
    unsigned char *pSnr;
    pSnr = (unsigned char*)malloc(20*sizeof(unsigned char));
    pTagType = (unsigned char*)malloc(20*sizeof(unsigned char));

while(continue_reading){
    unsigned char status = PcdRequest(PICC_REQIDL,pTagType);
    std::cout << (int)status << std::endl;
    if(status == TAG_OK){
        std::cout<<"Card yes!"<<std::endl;
        status = PcdAnticoll(0, pSnr); //prima chestie whatever
        if (status == TAG_OK){
            std::cout<<pSnr;
        }
      }
      sleep(1);
    }
  }
