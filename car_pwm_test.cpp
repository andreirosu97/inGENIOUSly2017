#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>
#include <iostream>
#include <stdlib.h>

int main()
{
  int i;
  if(wiringPiSetup()==-1){
    printf("wiringPi sefut error\n");
    return 1;
  }
  pinMode(1,PWM_OUTPUT);/*
  pwmSetClock(2);
  pwmSetRange(10);*/
  pwmWrite(1,5);/*
  while(true)
  {
    pwmSetRange(1000);
    delay(500);
    pwmSetRange(10);
    delay(500);
  }*/
  return 0;
}
