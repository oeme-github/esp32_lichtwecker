#pragma once

#ifndef __LIBDEBUG_H__
#define __LIBDEBUG_H__

/** 
 * Define DEBUG_SERIAL_ENABLE to enable debug serial. 
 * Comment it to disable debug serial. 
 */
#define DEBUG_SERIAL_ENABLE false

/**
 * Define dbSerial for the output of debug messages. 
 */
#define dbSerial Serial
#define dbSerialSpeed 115200

#ifdef DEBUG_SERIAL_ENABLE
#define dbSerialBegin(a)       dbSerial.begin(a)
#define dbSerialPrint(a)       dbSerial.print(a)
#define dbSerialPrintln(a)     dbSerial.println(a)
#define dbSerialPrintHex(a)    dbSerial.print(a, HEX)
#define dbSerialPrintf(a, ...)                                                                    \
   do                                                                                             \
   {                                                                                              \
      dbSerial.printf("%lu:%s:%d:%s - " a, millis(), __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__); \
      dbSerial.println(); \
   } while (0)

#else
#define dbSerialPrint(a)    do{}while(0)
#define dbSerialPrintln(a)  do{}while(0)
#define dbSerialPrintHex(a) do{}while(0)
#define dbSerialBegin(a)    do{}while(0)
#define dbSerialPrintf(a)    do{}while(0)
#endif

/**
 * @}
 */

#endif