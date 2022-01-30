#include <Nextion.h>
#include <ESPNexUpload.h>
#include <CallbackFunctions.h>
#include <list>

#ifndef __NEXTIONDISPL_H__
#define __NEXTIONDISPL_H__


/**
 * @brief define pages
 * 
 */

typedef enum typeOfFunc 
{
    push,
    pop,
    cmd    
} typeOfFunc_t;

/* pages */
typedef struct Pages_t {
    std::vector <std::pair<int,const char*>> page_vec={
            { 0,"page0" },
            { 1,"page1" },
            { 2,"page2" },
            { 3,"page3" },
            { 4,"page4" },
            { 5,"page5" }
    };    
    std::vector <std::pair<int,const char*>>::iterator it_page_vec;

    /* DS buttons */
    std::vector<std::tuple<int,int,const char*>> btn_vec={
        std::tuple<int,int,const char*>{ 0, 23, "btSound"    },
        std::tuple<int,int,const char*>{ 2, 26, "btTimeSync" },
        std::tuple<int,int,const char*>{ 3,  2, "btSnooze"   }
    };
    /* varaibales */
    std::vector <std::tuple<int, int,const char*>> var_vec={
        std::tuple<int,int,const char*>{ 0, 30, "vaOffsetSun" },
        std::tuple<int,int,const char*>{ 0, 38, "vaBright"},
        std::tuple<int,int,const char*>{ 2, 31, "vaVolume"    },
    };
    /* timers */
    std::vector <std::tuple<int, int,const char*>> tim_vec={
        std::tuple<int,int,const char*>{ 0, 28, "tmSerial" },
        std::tuple<int,int,const char*>{ 3,  4, "tmPage3"  }
    };

    std::vector <std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>> callback_vec={
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btSound"    , push, page0_btSoundPushCallback    },
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "tmSerial"   , cmd , page0_tmSerialCmdCallback    }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "tmPage3"    , cmd , page0_tmSerialCmdCallback    }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btTimeSync" , push, page2_btTimeSyncPushCallback },
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btSnooze"   , push, page3_btSnoozePushCallback   },
    };
} PAGES_T;

class NextionDisplay
{
private:
    /* data */
    ESPNexUpload *nexUplSrv;
    /**
     * objects for touch event
     */
    PAGES_T pages;
    /* pages */
    std::vector<NexPage *> pages_vec; 
    /* buttons */
    std::vector<NexDSButton *> buttons_vec; 
    /* varaibles */
    std::vector<NexVariable *> vars_vec; 
    /* timers */
    std::vector<NexTimer*> timers_vec; 
    /* touch */
    std::vector<NexTouch *> nex_listen_list_vec;
    // RTC
    NexRtc  nexRtc;

public:
    NextionDisplay(int iSerialSpeed_);
    NextionDisplay();
    ~NextionDisplay();

    void InitDisplay();
    /* getter & setter */
    std::vector<NexTouch *> getNexListenList();
    std::vector<NexTimer *> getNexTimer();
    std::vector<NexVariable *> getNexVariable(); 
    NexVariable *getNexVariableByName(const char * name_); 
    NexDSButton *getNexButtonByName(const char * name_);

private:
    /* page factory */
    static NexPage* Create_Pages(int8_t iPage_, const char *name_);
    static NexDSButton* Create_Btns(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexVariable* Create_Vars(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexTimer*    Create_Timers(int8_t iPage_, int8_t iElemet_, const char *name_);
    /* init function */
    void initPages();
    /* add page to pages_vec */
    void addPage(NexPage *nexPage_);
    void addBtn(NexDSButton *nexBtn_);
    void addVar(NexVariable *nexVar_);
    void addTimer(NexTimer *nexTimer_);
    /* add element to nex_listen_list_vec */
    void addToList(NexTouch *nexTouch_);
    /* attach push-callback functions */
    void attachFunc(const char *, typeOfFunc_t, NexTouchEventCb);
};

#endif