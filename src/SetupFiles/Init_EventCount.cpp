#include "TC_EventCount.h"

//clock Definitions
enum GCLK_IDs {

// '*' means its set up by Arduino core 
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


void init_EventCount(SodaqRTC rtc)
{
//All Set up funcitons called from here

   init_GCLK();                             
   init_EIC();
   init_EVSYS();
   init_TC();
   init_RTC(rtc);
}



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


void init_RTC(SodaqRTC rtc)
{
   rtc.begin();
   RTC->MODE2.CLOCK.reg = 0;
   RTC->MODE2.Mode2Alarm[0].ALARM.reg = sleepTime;
   rtc.enableAlarm(rtc.MATCH_HHMMSS);
   rtc.attachInterrupt(RTC_ISR);
}


void init_EIC()
{
   /*
*  Soemthing in the GPIO/EIC setup code is causing this to break.
*  When it breaks it hangs w/cortex handler inf loop.
*
*
   NVIC_DisableIRQ(EIC_IRQn);
   NVIC_ClearPendingIRQ(EIC_IRQn);
   NVIC_SetPriority(EIC_IRQn, 0);
   NVIC_EnableIRQ(EIC_IRQn);

   GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_EIC));
   EIC->CTRL.bit.ENABLE = enable;
   while(EICisSyncing);

   EIC->WAKEUP.reg |= EIC_WAKEUP_WAKEUPEN6 | EIC_WAKEUP_WAKEUPEN8 | EIC_WAKEUP_WAKEUPEN9;


   //Configure Pins
   PORT->Group[1].PINCFG[8].reg =  PORT_PINCFG_INEN |   //Input Enable
                                       PORT_PINCFG_PULLEN;  //Pull Resistor

   PORT->Group[1].PINCFG[9].reg =  PORT_PINCFG_INEN |   //Input Enable
                                       PORT_PINCFG_PULLEN;  //Pull Resistor

   PORT->Group[0].PINCFG[6].reg =  PORT_PINCFG_INEN |   //Input Enable
                                       PORT_PINCFG_PULLEN;  //Pull Resistor

   PORT->Group[0].DIRCLR.reg = PORTA6;
   PORT->Group[1].DIRCLR.reg = PORTB8 | PORTB9;
   PORT->Group[0].OUTCLR.reg = PORTA6;
   PORT->Group[1].OUTCLR.reg = PORTB8 | PORTB9;

   //Assign Pins A1,A2,A3 to EXTINT8,EXTINT9,EXTINT6.
   PORT->Group[1].PMUX[8].bit.PMUXO = PORT_PMUX_PMUXE(0);
   PORT->Group[1].PINCFG[8].reg |= PORT_PINCFG_PMUXEN;   //PMUX on

   PORT->Group[1].PMUX[9].bit.PMUXE = PORT_PMUX_PMUXO(0);
   PORT->Group[1].PINCFG[9].reg |= PORT_PINCFG_PMUXEN;   //PMUX on

   PORT->Group[0].PMUX[6].bit.PMUXE = PORT_PMUX_PMUXE(0);
   PORT->Group[0].PINCFG[6].reg |= PORT_PINCFG_PMUXEN;   //PMUX on

   //Configre External Interrupts
   EIC->CONFIG[1].reg |=   EIC_CONFIG_SENSE0_RISE; // |   // Trigger on Rising Edge
   EIC->CONFIG[1].reg |=   EIC_CONFIG_SENSE1_RISE; // |   // Tigger on Rising Edge
   EIC->CONFIG[0].reg |=   EIC_CONFIG_SENSE6_RISE; // |   // Trigger on Rising Edge

   EIC->INTENSET.reg |=    EIC_INTENSET_EXTINT8 |     //Enable EXTINT8
                           EIC_INTENSET_EXTINT9 |     //Enable EXTINT9
                           EIC_INTENSET_EXTINT6;      //Enable EXTINT6

*/

   PM->APBAMASK.reg |= PM_APBAMASK_EIC;    //Enable EIC in Power Manager

   EIC->CTRL.bit.ENABLE = disable;           //Disable to allow for some register writes
   while(EICisSyncing);


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
