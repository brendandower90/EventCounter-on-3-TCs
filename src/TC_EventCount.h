#ifndef TC_EVENT_COUNT_H
#define TC_EVENT_COUNT_H

/*
  TC_EventCount.h - Main include file for the TC_EventCount sketch

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This sketch is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/

#include <Arduino.h>
#include <RTCZero.h>
//#include <clocks.h>

//Function Declarations
void init_GCLK();
void init_EIC();
void init_TC();
void init_EVSYS();
void init_RTC(SodaqRTC rtc);
void RTC_ISR();
void printEpochTime(SodaqRTC rtc);
void EIC_ISR();
void init_TCC2();

//Serial Macros
#define  debug(x)       SerialUSB.print(x)
#define  debugl(x)      SerialUSB.println(x)
#define  endl           SerialUSB.print("\n")
#define  tab            SerialUSB.print("\t")
#define  nbsp           SerialUSB.print(" ")
#define  flush          SerialUSB.flush();


//Syncing Definitions
#define  TC3isSyncing   TC3->COUNT16.STATUS.bit.SYNCBUSY
#define  TC4isSyncing   TC4->COUNT16.STATUS.bit.SYNCBUSY
#define  TC5isSyncing   TC5->COUNT16.STATUS.bit.SYNCBUSY
#define  EICisSyncing   EIC->STATUS.bit.SYNCBUSY
#define  GCLKisSyncing  GCLK->STATUS.bit.SYNCBUSY

//Event Channels
#define  EXTINT8_EVENT  0
#define  EXTINT9_EVENT  1
#define  EXTINT6_EVENT  2

//PIN assignments
#define  PORTB8   (1<<8)
#define  PORTB9   (1<<9) 
#define  PORTA6   (1<<6)

//Enable/Disable
#define  enable   1
#define  disable  0

//RTC Global Variables


#define rtc_resetTime() rtc.setAlarmTime(0,0,0)  
extern volatile uint32_t epochTime;
extern const int sleepTime;
extern volatile int nextAlarm;
extern volatile bool RTCTriggered;;

//Clocks
#define CONF_CLOCK_OSC8M_ON_DEMAND = true;
#define CONF_CLOCK_OSC8M_RUN_IN_STANDBY = true;



#endif