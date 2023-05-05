
#ifndef __DEBUGLIB_H__
#define __DEBUGLIB_H__

/** 
 * Define DEBUG_SERIAL_ENABLE to enable debug serial. 
 * Comment it to disable debug serial. 
 */
#define DEBUG_SERIAL_ENABLE

/**
 * Define dbSerial for the output of debug messages. 
 */
#define dbSerial Serial
#define dbSerialSpeed 115200

#ifdef DEBUG_SERIAL_ENABLE
#define dbSerialPrint(a)       dbSerial.print(a)
#define dbSerialPrintln(a)     dbSerial.println(a)
#define dbSerialBegin(a)       dbSerial.begin(a)
#define dbSerialPrintf(a, ...)                                                                    \
   do                                                                                             \
   {                                                                                              \
      dbSerial.printf("%lu: %s:%d - " a, millis(), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); \
   } while (0)

#else
#define dbSerialPrint(a)    do{}while(0)
#define dbSerialPrintln(a)  do{}while(0)
#define dbSerialBegin(a)    do{}while(0)
#define dbSerialPrintf(a)    do{}while(0)
#endif

/**
 * @}
 */

#endif