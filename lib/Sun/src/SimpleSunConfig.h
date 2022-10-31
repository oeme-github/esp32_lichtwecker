
#ifndef NUM_LEDS
#define NUM_LEDS 14     //number of LEDs in the strip
#endif
#ifndef BRIGHTNESS      
#define BRIGHTNESS 255  //strip brightness 255 max
#endif
#ifndef WAKE_DELAY
#define WAKE_DELAY 1800 //sunrise span in sec
#endif
#ifndef SUN_PHASE
#define SUN_PHASE 1000   // cound of sun phases 
#endif
#ifndef WHITE_LEVEL
#define WHITE_LEVEL 10 // max. white level
#endif
#ifndef RED_LEVEL
#define RED_LEVEL 255 // max. red level
#endif
#ifndef GREEN_LEVEL
#define GREEN_LEVEL 100 // max. green level
#endif
#ifndef BLUE_LEVEL
#define BLUE_LEVEL 10 // max. blue level
#endif


#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

//#define DEBUG_SERIAL_SUN_ENABLE
//#define _WITH_TEST_LED_
//#define _DEBUG_SUN_DETAILS_

#ifdef DEBUG_SERIAL_SUN_ENABLE
    #define dbSunSerialPrint(a)    Serial.print(a)
	#define dbSunSerialPrintHex(a) Serial.print(a, HEX)
    #define dbSunSerialPrintln(a)  Serial.println(a)
    #define dbSunSerialBegin(a)    Serial.begin(a)
#else
    #define dbSunSerialPrint(a)    do{}while(0)
    #define dbSunSerialPrintHex(a) do{}while(0)		
    #define dbSunSerialPrintln(a)  do{}while(0)
    #define dbSunSerialBegin(a)    do{}while(0)
#endif
