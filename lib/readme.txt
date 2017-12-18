Author: brendan90
License: Included in TC_EventCount.h

Aim:  To count how many pulses sensor reads occur on pins A2, A3 and A6 whilst CPU
      is in standby. Wakeup CPU every minute to report number of sensor reads

Functional Description:
   1. CPU is put to sleep
   2. Each time a sensor is triggered it triggers an event in the event system. 
   3. This event system routes the event to one of 3 timers, one per sensor. 
   3. When the timer receives the event the timer increments its 'count' register.     
   4. A real time clock (RTC) wakes up the device at a specific time
   5. The program once awake returns the number of reads of each sensor and resets
    
Files & Dependencies:
   1. TC_EventCount.cpp  
   2. TC_EventCount.h
   3. Init_EventCount.cpp
   4. RTCZero.h

Installation:  Put all files in the same folder. Compile this file TC_EventCount.cpp
               Compile from Arduino Editor, or with Platformio.
               Replace "epoch" with actual epoch time from www.epochconverter.com

Known Issues:
   1. Serial Display shows a space before every 2nd print out.
   2. LED_GREEN may not exist on the Arduino Zero, change to LED_BUILTIN or other LED
   3. Pin Configuration done with Arduino attachInterrupt() function. Using registers
      causes an infinite loop on cortex_handlers.c
      
