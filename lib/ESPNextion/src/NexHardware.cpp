/**
 * @file NexHardware.cpp
 *
 * The implementation of base API for using Nextion. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/11
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexHardware.h"

#define NEX_RET_CMD_FINISHED                (0x01)
#define NEX_RET_EVENT_LAUNCHED              (0x88)
#define NEX_RET_EVENT_UPGRADED              (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)     
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_INVALID_CMD                 (0x00)
#define NEX_RET_INVALID_COMPONENT_ID        (0x02)
#define NEX_RET_INVALID_PAGE_ID             (0x03)
#define NEX_RET_INVALID_PICTURE_ID          (0x04)
#define NEX_RET_INVALID_FONT_ID             (0x05)
#define NEX_RET_INVALID_BAUD                (0x11)
#define NEX_RET_INVALID_VARIABLE            (0x1A)
#define NEX_RET_INVALID_OPERATION           (0x1B)
// extention
#define NEX_REC_EVENT_TOUCH_HEAD            (0x23)
#define NEX_REC_LOG_MSG_HEAD                (0x25)
#define NEX_REC_EVENT_DELIMTER_HEAD         (0x3b) 

/*
 * Receive uint32_t data. 
 * 
 * @param number - save uint32_t data. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.
 *
 */
bool recvRetNumber(uint32_t *number, uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[8] = {0};

    uint8_t iTempSize;
    uint8_t iBuffRead;

    if (!number)
    {
        goto __return;
    }
    
    nexSerial.setTimeout(timeout);

    iTempSize = sizeof(temp);
    iBuffRead = nexSerial.readBytes((char *)temp, sizeof(temp));

    if( iTempSize != iBuffRead )
    {
        goto __return;
    }

    if (temp[0] == NEX_RET_NUMBER_HEAD
        && temp[5] == 0xFF
        && temp[6] == 0xFF
        && temp[7] == 0xFF
        )
    {
        *number = ((uint32_t)temp[4] << 24) | ((uint32_t)temp[3] << 16) | (temp[2] << 8) | (temp[1]);
        ret = true;
    }

__return:
    
    return ret;
}


/*
 * Receive string data. 
 * 
 * @param buffer - save string data. 
 * @param len - string buffer length. 
 * @param timeout - set timeout time. 
 *
 * @return the length of string buffer.
 *
 */
uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout)
{
    uint16_t ret = 0;
    bool str_start_flag = false;
    uint8_t cnt_0xff = 0;
    String temp = String("");
    uint8_t c = 0;
    long start;

    if (!buffer || len == 0)
    {
        goto __return;
    }
    
    start = millis();
    while (millis() - start <= timeout)
    {
        while (nexSerial.available())
        {
            c = nexSerial.read();
            if (str_start_flag)
            {
                if (0xFF == c)
                {
                    cnt_0xff++;                    
                    if (cnt_0xff >= 3)
                    {
                        break;
                    }
                }
                else
                {
                    temp += (char)c;
                }
            }
            else if (NEX_RET_STRING_HEAD == c)
            {
                str_start_flag = true;
            }
        }
        
        if (cnt_0xff >= 3)
        {
            break;
        }
    }

    ret = temp.length();
    ret = ret > len ? len : ret;
    strncpy(buffer, temp.c_str(), ret);
    
__return:

    dbSerialPrint("recvRetString[");
    dbSerialPrint(temp.length());
    dbSerialPrint(",");
    dbSerialPrint(temp);
    dbSerialPrintln("]");

    return ret;
}

/*
 * Send command to Nextion.
 *
 * @param cmd - the string of command.
 */
void sendCommand(const char* cmd)
{
    while (nexSerial.available())
    {
        nexSerial.read();
    }
    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}


/*
 * Command is executed successfully. 
 *
 * @param timeout - set timeout time.
 *
 * @retval true - success.
 * @retval false - failed. 
 *
 */
bool recvRetCommandFinished(uint32_t timeout)
{    
    bool ret = false;
    uint8_t temp[4] = {0};
    nexSerial.setTimeout(timeout);

    size_t count = nexSerial.readBytes((char *)temp, sizeof(temp));

    if (sizeof(temp) != count)
    {
        ret = false;
    }
    if (temp[0] == NEX_RET_CMD_FINISHED
        && temp[1] == 0xFF
        && temp[2] == 0xFF
        && temp[3] == 0xFF
        )
    {
        ret = true;
    }
    return ret;
}


bool nexInit(void)
{
    bool ret1 = false;
    bool ret2 = false;
    
    nexSerial.begin(nexSerialSpeed);
    delay(200);
    sendCommand("");
    sendCommand("bkcmd=1");
    ret1 = recvRetCommandFinished( 200 );
    if(ret1 == false)
    {
        dbSerialPrintln("sendCommand bkcmd=1 failed!");
    }
    sendCommand("page 0");
    ret2 = recvRetCommandFinished( 200 );
    if(ret2 == false)
    {
        dbSerialPrintln("sendCommand page 0 failed!");
    }
    // check results
    return ret1 && ret2;
}


void nexLoop(NexTouch *nex_listen_list[], uint8_t sizeOfList)
{
    static uint8_t __buffer[10];
    String data_from_display="";
    uint8_t __pid=-1;
    uint8_t __cid=-1;
    
    uint16_t i=0;
    uint8_t c;  

    while (nexSerial.available() > 0)
    {   
        delay(10);
        c = nexSerial.read();
        
        if (NEX_RET_EVENT_TOUCH_HEAD == c)
        {
            if (nexSerial.available() >= 6)
            {
                __buffer[0] = c;  
                for (i = 1; i < 7; i++)
                {
                    __buffer[i] = nexSerial.read();
                }
                __buffer[i] = 0x00;
                
                if (0xFF == __buffer[4] && 0xFF == __buffer[5] && 0xFF == __buffer[6])
                {
                    NexTouch::iterate(nex_listen_list, __buffer[1], __buffer[2], (int32_t)__buffer[3], "", sizeOfList);
                }                
            }
        }
        // receive an event
        // "#14;0;sunup#"
        if( NEX_REC_EVENT_TOUCH_HEAD == c )
        {
#ifdef _WITH_NEXLOOP_DEBUG_
            dbSerialPrintln("receiving a cmd...");
#endif
            while(nexSerial.available())
            {
                // read from serial
                c = nexSerial.read();

                if(c == NEX_REC_EVENT_DELIMTER_HEAD)
                {
                    i++;
                    // found ;
                    if(i==1)
                    {
                        __pid=atoi(data_from_display.c_str());
                        data_from_display="";
                    }
                    else if(i==2)
                    {
                        __cid=atoi(data_from_display.c_str());
                        data_from_display="";
                    }
                }
                else if(NEX_REC_EVENT_TOUCH_HEAD == c)
                {
                    // found # -> end if cmd
#ifdef _WITH_NEXLOOP_DEBUG_
                    dbSerialPrint("pid:");
                    dbSerialPrint(String(__pid));
                    dbSerialPrint(" / cid:");
                    dbSerialPrint(String(__cid));
                    dbSerialPrint(" / cmd:");
                    dbSerialPrintln(data_from_display);
#endif
                    NexTouch::iterate(nex_listen_list, __pid, __cid, (int32_t)NEX_REC_EVENT_TOUCH_HEAD, data_from_display, sizeOfList); 
                    data_from_display="";   
                    // leave while        
                    break;
                }
                else
                {
                    data_from_display += char(c);
                }
           }
        }
        // receive a log message
        if( NEX_REC_LOG_MSG_HEAD == c )
        {
#ifdef _WITH_NEXLOOP_DEBUG_
            dbSerialPrint("receiving a log message: ");
#endif
            while(nexSerial.available())
            {
                c = nexSerial.read();
                if(c==NEX_REC_LOG_MSG_HEAD)
                {
#ifdef _WITH_NEXLOOP_DEBUG_
                    dbSerialPrintln(data_from_display);
#endif
                    data_from_display="";    
                    break;   
                }
                else
                {
                    data_from_display += char(c);
                }
            }
        }
    }
}
