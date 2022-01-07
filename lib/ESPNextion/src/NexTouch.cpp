/**
 * @file NexTouch.cpp
 *
 * The implementation of class NexTouch. 
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
#include "NexTouch.h"

NexTouch::NexTouch(uint8_t pid, uint8_t cid, const char *name)
    :NexObject(pid, cid, name)
{
    this->__cb_push = NULL;
    this->__cbpush_ptr = NULL;

    this->__cb_pop = NULL;
    this->__cbpop_ptr = NULL;

    this->__cb_cmd_e = NULL;
    this->__cbcmd_e_ptr = NULL;
}

void NexTouch::attachPush(NexTouchEventCb push, void *ptr)
{
    this->__cb_push = push;
    this->__cbpush_ptr = ptr;
}

void NexTouch::detachPush(void)
{
    this->__cb_push = NULL;
    this->__cbpush_ptr = NULL;
}

void NexTouch::attachPop(NexTouchEventCb pop, void *ptr)
{
    this->__cb_pop = pop;
    this->__cbpop_ptr = ptr;
}

void NexTouch::detachPop(void)
{
    this->__cb_pop = NULL;    
    this->__cbpop_ptr = NULL;
}

void NexTouch::push(void)
{
    if (__cb_push)
    {
        __cb_push(__cbpush_ptr);
    }
}

void NexTouch::pop(void)
{
    if (__cb_pop)
    {
        __cb_pop(__cbpop_ptr);
    }
}

void NexTouch::cmd_e(void)
{
    if (__cb_cmd_e)
    {
        __cb_cmd_e(__cbcmd_e_ptr);
    }
}

void NexTouch::attachCmd(NexTouchEventCb cmd_e, void *ptr)
{
    this->__cb_cmd_e = cmd_e;
    this->__cbcmd_e_ptr = ptr;
}

void NexTouch::detachCmd(void)
{
    this->__cb_cmd_e = NULL;
    this->__cbcmd_e_ptr = NULL;
}

void NexTouch::iterate(NexTouch **list, uint8_t pid, uint8_t cid, int32_t event, String cmd)
{
    NexTouch *e = NULL;
    uint16_t i = 0;

    if (NULL == list)
    {
#ifdef _WITH_NEXLOOP_DEBUG_
        dbSerialPrint("list is NULL");
#endif    
        return;
    }

#ifdef _WITH_NEXLOOP_DEBUG_
    dbSerialPrint("event:");
    dbSerialPrintln(event);
#endif    
    for(i = 0; (e = list[i]) != NULL; i++)
    {
        if (e->getObjPid() == pid && e->getObjCid() == cid)
        {
            e->printObjInfo();
            if (NEX_EVENT_PUSH == event)
            {
#ifdef _WITH_NEXLOOP_DEBUG_
                dbSerialPrintln("call push callback...");
#endif
                e->push();
            }
            else if (NEX_EVENT_POP == event)
            {
#ifdef _WITH_NEXLOOP_DEBUG_
                dbSerialPrintln("call pop callback...");
#endif
                e->pop();
            }            
            else if (NEX_EVENT_CMD == event)
            {
#ifdef _WITH_NEXLOOP_DEBUG_
                dbSerialPrint("call cmd callback: ");
                dbSerialPrintln( cmd );
#endif
                /* param */
                e->__cbcmd_e_ptr=(void *)&cmd;
                /* function call */
#ifdef _WITH_NEXLOOP_DEBUG_
                dbSerialPrint("Name: ");
                dbSerialPrintln( e->getName() );
#endif
                e->cmd_e();
            }            
            break;
        }
    }
}

