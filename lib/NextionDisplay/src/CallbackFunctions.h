#include <Nextion.h>

#ifndef __CALLBACK_FUNCTIONS_H__
#define __CALLBACK_FUNCTIONS_H__

/** 
 * === NEXTION CALLBACK SECTION ===
 */

void page0_btSoundPushCallback(void *ptr);
void page2_btTimeSyncPushCallback(void *ptr);
void page3_btSnoozePushCallback(void *ptr);
void page3_btLightPushCallback(void *ptr);
void page0_tmSerialCmdCallback(void *ptr);

#endif