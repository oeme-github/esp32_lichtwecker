/**
 * @file NexConfig.h
 *
 * Options for user can be found here. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/13
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <libDebug.h>

#ifndef __NEXCONFIG_H__
#define __NEXCONFIG_H__

/**
 * @addtogroup Configuration 
 * @{ 
 */

/** 
 * Define DEBUG_SERIAL_ENABLE to enable debug serial. 
 * Comment it to disable debug serial. 
 */
//#define DEBUG_SERIAL_ENABLE
//#define _WITH_NEXLOOP_DEBUG_
//#define _WITH_NEXLOOP_DETAIL_DEBUG_

/**
 * Define dbSerial for the output of debug messages. 
 */
//#define dbSerial Serial
//#define dbSerialSpeed 115200
/**
 * Define nexSerial for communicate with Nextion touch panel. 
 */
#define nexSerial Serial2
#define nexSerialSpeed 115200

//#ifdef DEBUG_SERIAL_ENABLE
//#define dbSerialPrint(a)    dbSerial.print(a)
//#define dbSerialPrintln(a)  dbSerial.println(a)
//#define dbSerialBegin(a)    dbSerial.begin(a)
//#else
//#define dbSerialPrint(a)    do{}while(0)
//#define dbSerialPrintln(a)  do{}while(0)
//#define dbSerialBegin(a)    do{}while(0)
//#endif

/**
 * @}
 */

#endif /* #ifndef __NEXCONFIG_H__ */
