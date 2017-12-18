#include "TC_EventCount.h"

//clock Definitions
enum GCLK_IDs {

//* means its being used by Arduino core 
// Can get rid of one of the 1ms clocks

   GCLK0,   //48MHz  DFLL     (CPU Clock)*
   GCLK1,   //32kHz  XOSC32   (DFLL Source)*
   GCLK2,   //1kHz   ULP32    (RTC - 1ms)
   GCLK3,   //8MHz   OSC8M    (NONE)*
   GCLK4,   //16MHz  ULP32    (EventCount - 1ms)
   GCLK5,
   GCLK6,
   GCLK7,

};

void init_GCLK()
{   

   GCLK->GENCTRL.reg =  GCLK_GENCTRL_ID(GCLK4) |      // Select GCLK 1 Generator
                        GCLK_GENCTRL_SRC_OSCULP32K |    // Assign 8MHz Osccilator
                        GCLK_GENCTRL_RUNSTDBY |       // Allow it to run in standby
                        GCLK_GENCTRL_DIVSEL |         // Slow down the clock speed
                        GCLK_GENCTRL_IDC |            // Set duty cycle to 50%
                        GCLK_GENCTRL_GENEN;           // Enable it.                 
   while (GCLKisSyncing);  

   GCLK->GENDIV.reg =   GCLK_GENDIV_ID(GCLK4) |
                        GCLK_GENDIV_DIV(32);          //1kHz (1ms)

   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |     // Select GCLK1
                        GCLK_CLKCTRL_ID_EIC |         // Assign it to EIC
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while(GCLKisSyncing);

   //Assign GCLK1 to EVSYS_Ch-0, 1, 2
   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |      // Select GCLK1
                        GCLK_CLKCTRL_ID_EVSYS_0 |     // Assign it to EVSYS Ch-0
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while(GCLKisSyncing);

   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |      // Select GCLK1
                        GCLK_CLKCTRL_ID_EVSYS_1 |     // Assign it to EVSYS Ch-0
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while(GCLKisSyncing);

   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |      // Select GCLK1
                        GCLK_CLKCTRL_ID_EVSYS_2 |     // Assign it to EVSYS Ch-0
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while(GCLKisSyncing);


   //Assign GCLK3 to TCs
   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |     // Select GCLK5,
                        GCLK_CLKCTRL_ID_TCC2_TC3 |    // Assign it to TCC2 and TC3,
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while (GCLKisSyncing);  

   GCLK->CLKCTRL.reg =  GCLK_CLKCTRL_GEN(GCLK4) |     // Select GCLK5,
                        GCLK_CLKCTRL_ID_TC4_TC5 |     // Assign it to TCC2 and TC3,
                        GCLK_CLKCTRL_CLKEN;           // Enable it.
   while (GCLKisSyncing);  

}


void init_RTC(RTCZero rtc)
{

   
   rtc.begin();
   RTC->MODE2.CLOCK.reg = 0;
   RTC->MODE2.Mode2Alarm[0].ALARM.reg = sleepTime;
   rtc.enableAlarm(rtc.MATCH_HHMMSS);
   rtc.attachInterrupt(RTC_ISR);

}


void init_EIC()
{
   
   PM->APBAMASK.reg |= PM_APBAMASK_EIC;    //Enable EIC in Power Manager

   EIC->CTRL.bit.ENABLE = disable;           //Disable to allow for some register writes
   while(EICisSyncing);

   //Initialise Pins & Interrupts
   pinMode(A2, INPUT_PULLDOWN);
   pinMode(A3, INPUT_PULLDOWN);
   pinMode(A6, INPUT_PULLDOWN);
   attachInterrupt(A2, NULL, HIGH);               // Attach interrupts on PIN A2
   attachInterrupt(A3, NULL, HIGH);                 // Attach Interrupts on PIN A3
   attachInterrupt(A6, NULL, HIGH);                 // Attach interrupts on PIN A6

   EIC->WAKEUP.bit.WAKEUPEN6 = disable;
   EIC->WAKEUP.bit.WAKEUPEN8 = disable;
   EIC->WAKEUP.bit.WAKEUPEN9 = disable;

   EIC->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO8;           // Set Interrupt EXTINT08 for Event Generation
   EIC->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO9;           // Set Interrupt EXTINT09 for Event Generation
   EIC->EVCTRL.reg |= EIC_EVCTRL_EXTINTEO6;           // Set Interrupt EXTINT06 for Event Generation

   
   EIC->CTRL.bit.ENABLE = enable;
   while(EICisSyncing);

}


void init_EVSYS()
{
   PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;    //Enable in Power Manager

   //Route Event Users (Outputs)
   REG_EVSYS_USER =  EVSYS_USER_CHANNEL(EXTINT8_EVENT + 1) |                // Select Channel of EXTINT8, 
                     EVSYS_USER_USER(EVSYS_ID_USER_TC3_EVU);                // Route it to user (output) timer TC3
    
   REG_EVSYS_USER =  EVSYS_USER_CHANNEL(EXTINT9_EVENT + 1) |                // Select Channel of EXTINT8, 
                     EVSYS_USER_USER(EVSYS_ID_USER_TC4_EVU);                // Route it to user (output) timer TC4

   REG_EVSYS_USER =  EVSYS_USER_CHANNEL(EXTINT6_EVENT +1) |                 // Select Channel of EXTINT8, 
                     EVSYS_USER_USER(EVSYS_ID_USER_TC5_EVU);                // Route it to user (output) timer TC5


   //Set up Event Channels (Inputs)
   EVSYS->CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT|                // No event edge detection
                        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                  // Set event path as asynchronous
                        EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_8) |   // Set event generator (input) as external interrupt 8
                        EVSYS_CHANNEL_CHANNEL(EXTINT8_EVENT);
                      
   EVSYS->CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |               // No event edge detection
                        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                  // Set event path as asynchronous
                        EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_9) |   // Set event generator (input) as external interrupt 9
                        EVSYS_CHANNEL_CHANNEL(EXTINT9_EVENT);
                    
   EVSYS->CHANNEL.reg = EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT |               // No event edge detection
                        EVSYS_CHANNEL_PATH_ASYNCHRONOUS |                  // Set event path as asynchronous
                        EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_6) |   // Set event generator (input) as external interrupt 6
                        EVSYS_CHANNEL_CHANNEL(EXTINT6_EVENT);                          

}

void init_TC()
{
   //Enable in Power Manager
   PM->APBCMASK.reg |= PM_APBCMASK_TC3 | PM_APBCMASK_TC4 | PM_APBCMASK_TC5; 

   //Enable Events on Counters
   TC3->COUNT16.EVCTRL.reg |= TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_COUNT;      // Set up TC3 to count on event
   TC4->COUNT16.EVCTRL.reg |= TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_COUNT;      // Set up TC4 to count on event              
   TC5->COUNT16.EVCTRL.reg |= TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_COUNT;      // Set up TC5 to count on event


   //Set up Timers TC3, TC4, TC5
   TC3->COUNT16.CTRLA.reg |=  TC_CTRLA_PRESCALER_DIV16 |    // Set prescaler to 16, 16MHz/16 = 1MHz
                              TC_CTRLA_RUNSTDBY |           // Set to run in standby
                              TC_CTRLA_ENABLE;              // Enable TC5
   while (TC3isSyncing); 

   TC4->COUNT16.CTRLA.reg |=  TC_CTRLA_PRESCALER_DIV16 |    // Set prescaler to 16, 16MHz/16 = 1MHz
                              TC_CTRLA_RUNSTDBY |           // Set to run in standby
                              TC_CTRLA_ENABLE;              // Enable TC5
   while (TC4isSyncing); 

   TC5->COUNT16.CTRLA.reg |=  TC_CTRLA_PRESCALER_DIV16 |    // Set prescaler to 16, 16MHz/16 = 1MHz
                              TC_CTRLA_RUNSTDBY |           // Set to run in standby
                              TC_CTRLA_ENABLE;              // Enable TC5
   while (TC5isSyncing); 

}

void init_TCC2()
{
   #define TCCisSyncing    TCC2->SYNCBUSY.bit.ENABLE

   PM->APBCMASK.reg |= PM_APBCMASK_TCC2;
   TCC2->CTRLA.bit.ENABLE = disable;

   uint32_t period = sleepTime * 1000;
   TCC2->PER.reg = period;                      // Set TOP value to sleepTime
   TCC2->INTENSET.reg |= TCC_INTENSET_OVF;      // Enable Overflow interrupt on TOP

   TCC2->CTRLA.reg = TCC_CTRLA_RUNSTDBY |
                     TCC_CTRLA_ENABLE;
   while(TCCisSyncing);

   TCC2->CTRLBCLR.bit.DIR = TCC_CTRLBCLR_DIR;
   TCC2->CTRLBSET.bit.CMD = TCC_CTRLBSET_CMD_RETRIGGER;


}