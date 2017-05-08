#include "rc522.h"
#include <iostream>
#include <wiringPi.h>
#include <cstring>
#include <wiringPiSPI.h>
int continue_reading = 1 ;

int main(){
    std::cout<<"Welcome to the MFRC522 data read example"<<std::endl;
    std::cout<<"Press Ctrl-C to stop."<<std::endl;
    wiringPiSetup();
    wiringPiSPISetup (0, 10000000);
    unsigned char *pTagType;
    unsigned char *pSnr;
    pSnr = (unsigned char*)malloc(20*sizeof(unsigned char));
    pTagType = (unsigned char*)malloc(20*sizeof(unsigned char));

while(continue_reading){
    unsigned char status = MFRC522_Request(PICC_REQALL,pTagType);
    std::cout << (int)status;
    if(status == MI_OK){
        std::cout<<"Card yes!"<<std::endl;
        status = MFRC522_Anticoll(pSnr);
        if (status == MI_OK){
            std::cout<<pSnr;
        }
      }
    }
  }
