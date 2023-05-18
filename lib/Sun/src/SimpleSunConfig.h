#include <libDebug.h>

#ifndef NUM_LEDS
#define NUM_LEDS 28     //number of LEDs in the strip
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
#define WHITE_LEVEL 40 // max. white level
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
