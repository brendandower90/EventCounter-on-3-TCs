
/* ----------------------------- TC 3-Event Counter -----------------------------------
*  Author: brendan90
*  License: Included in TC_EventCount.h
*
*  Aim:  To count how many pulses sensor reads occur on pins A2, A3 and A6 whilst CPU
*        is in standby. Wakeup CPU every minute to report number of sensor reads
*
*  Functional Description:
*     1. CPU is put to sleep
*     2. Each time a sensor is triggered it triggers an event in the event system. 
*     3. This event system routes the event to one of 3 timers, one per sensor. 
*     3. When the timer receives the event the timer increments its 'count' register.     
*     4. A real time clock (RTC) wakes up the device at a specific time
*     5. The program once awake returns the number of reads of each sensor and resets
*    
*  Files & Dependencies:
*     1. TC_EventCount.cpp 
*     2. TC_EventCount.h
*     3. Init_EventCount.cpp
*     4. RTCZero.h
*
*  Installation:  Put all files in the same folder. Compile this file TC_EventCount.cpp
*                 Compile from Arduino Editor, or with Platformio.
*                 Replace "epoch" with actual epoch time from www.epochconverter.com
*
*  Known Issues:
*     1. Serial Display shows a space before every 2nd print out.
*     2. LED_GREEN may not exist on the Arduino Zero, change to LED_BUILTIN or other LED
*     3. Pin Configuration done with Arduino attachInterrupt() function. Using registers
*        causes an infinite loop on cortex_handlers.c
*
* -------------------------------------------------------------------------------------- */

#include "SetupFiles/TC_EventCount.h"


//Change this value to change the sleep Interval before next report
const int sleepTime = 5;

SodaqRTC rtc;
volatile int nextAlarm = sleepTime;
volatile bool RTCTriggered = false;


//Start
void setup() 
{
//Put your set up code here, to run once

   //Initialise and wait for Serial
   SerialUSB.begin(115200); 
   while(!SerialUSB);
   pinMode(LED_GREEN, OUTPUT);

   //Allow 10 sec for Re-upload of sketch
   for( int i = 1; i <= 5; i++){
      digitalWrite(LED_GREEN, LOW);   delay(1000);
      digitalWrite(LED_GREEN, HIGH);    delay(1000);
      SerialUSB.print(".");
   }
   SerialUSB.println("\n");

   //Configure EventCounter
   init_EventCount(rtc);

   //Set Sleep mode
   SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
   
}


void loop() {
// put your main code here, to run repeatedly:

   //Check if RTC Alarm has been triggered
   if (RTCTriggered){

      //Read TC3 Count
      nbsp;
      SerialUSB.print("Upper: ");
      TC3->COUNT16.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
      SerialUSB.print(TC3->COUNT16.COUNT.reg); 
      SerialUSB.print("\t");

      //Read TC4 Count
      SerialUSB.print("Lower: ");
      TC4->COUNT16.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
      SerialUSB.print(TC4->COUNT16.COUNT.reg);     
      SerialUSB.print("\t");

      //Read TC5 Count
      SerialUSB.print("Entry: ");
      TC5->COUNT16.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
      SerialUSB.print(TC5->COUNT16.COUNT.reg); 
      SerialUSB.println("\t");


      //Reset Counters;
      TC3->COUNT16.COUNT.reg = 0;
      TC4->COUNT16.COUNT.reg = 0;
      TC5->COUNT16.COUNT.reg = 0;

      //Set next Alarm for Wakeup
      RTC->MODE2.CLOCK.reg = 0;
      RTC->MODE2.Mode2Alarm[0].ALARM.reg = sleepTime;
      rtc.enableAlarm(rtc.MATCH_SS);

      RTCTriggered = false;
    
   }


   __WFI();   

}


void RTC_ISR()
{
   RTCTriggered = true;
   
}




