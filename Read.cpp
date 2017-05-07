#include "rc522.h"
int continue_reading = 1 ;

int main(){
    printf("Welcome to the MFRC522 data read example");
    printf("Press Ctrl-C to stop.");


while(continue_reading){
    unsigned char status = MFRC522_Request(PICC_REQIDL)
    std::pair<unsigned char, std::string> me;

    if(status == MI_OK){
        print "Card detected"
        me = MFRC522_Anticoll()
        if (status == MI_OK){
            std::cout<<me.second();
            MFRC522_Read(8);
        }
      }
    }
