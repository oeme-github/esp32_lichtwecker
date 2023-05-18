#pragma once

#include <Nextion.h>
#include <ESPNexUpload.h>
#include <CallbackFunctions.h>
#include <vector>

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
            { 0,"page 0" },
            { 1,"page 1" },
            { 2,"page 2" },
            { 3,"page 3" },
            { 4,"page 4" },
            { 5,"page 5" },
            { 6,"page 6" },
    };    
    std::vector <std::pair<int,const char*>>::iterator it_page_vec;

    /* DS buttons */
    std::vector<std::tuple<int,int,const char*>> btn_vec={
        std::tuple<int,int,const char*>{ 0, 23, "btSound"    },
        std::tuple<int,int,const char*>{ 2, 26, "btTimeSync" },
        std::tuple<int,int,const char*>{ 2, 33, "btReset"    },
        std::tuple<int,int,const char*>{ 3,  2, "btSnooze"   },
        std::tuple<int,int,const char*>{ 6, 19, "btLightTest"},
    };
    /* numbers */
    std::vector<std::tuple<int,int,const char*>> nmbr_vec={
        std::tuple<int,int,const char*>{ 6, 14, "nWhite"     },
        std::tuple<int,int,const char*>{ 6, 12, "nRed"       },
        std::tuple<int,int,const char*>{ 6, 10, "nGreen"     },
        std::tuple<int,int,const char*>{ 6,  2, "nBlue"      },
        std::tuple<int,int,const char*>{ 5, 15, "nDisBright" },
        std::tuple<int,int,const char*>{ 5, 13, "nBrightness"},
        std::tuple<int,int,const char*>{ 5,  3, "nMaxDimSR"  },
        std::tuple<int,int,const char*>{ 2,  4, "nOffsetSun" },
        std::tuple<int,int,const char*>{ 2, 10, "nSnooze"    },
        std::tuple<int,int,const char*>{ 2, 16, "nTimeout"   },
        std::tuple<int,int,const char*>{ 2, 24, "nDim"       },
        std::tuple<int,int,const char*>{ 2, 29, "nVolume"    },
        std::tuple<int,int,const char*>{ 1, 17, "nHour1"     },
        std::tuple<int,int,const char*>{ 1, 39, "nHour2"     },
        std::tuple<int,int,const char*>{ 1, 61, "nHour3"     },
        std::tuple<int,int,const char*>{ 1, 18, "nMin1"      },
        std::tuple<int,int,const char*>{ 1, 40, "nMin2"      },
        std::tuple<int,int,const char*>{ 1, 62, "nMin3"      },
    };
    /* radio */
    std::vector <std::tuple<int, int,const char*>> radio_vec={
        std::tuple<int,int,const char*>{ 4,  1, "r0"         },
        std::tuple<int,int,const char*>{ 4,  2, "r1"         },
        std::tuple<int,int,const char*>{ 4,  3, "r2"         },
    };
    /* checkbox */
    std::vector <std::tuple<int, int,const char*>> chkbx_vec={
        std::tuple<int,int,const char*>{ 1,  2, "cbOnOff1"   },
        std::tuple<int,int,const char*>{ 1, 24, "cbOnOff2"   },
        std::tuple<int,int,const char*>{ 1, 46, "cbOnOff3"   },
        std::tuple<int,int,const char*>{ 1,  4, "cbMo1"      },
        std::tuple<int,int,const char*>{ 1, 26, "cbMo2"      },
        std::tuple<int,int,const char*>{ 1, 48, "cbMo3"      },
        std::tuple<int,int,const char*>{ 1,  6, "cbDi1"      },
        std::tuple<int,int,const char*>{ 1, 28, "cbDi2"      },
        std::tuple<int,int,const char*>{ 1, 50, "cbDi3"      },
        std::tuple<int,int,const char*>{ 1,  8, "cbMi1"      },
        std::tuple<int,int,const char*>{ 1, 30, "cbMi2"      },
        std::tuple<int,int,const char*>{ 1, 52, "cbMi3"      },
        std::tuple<int,int,const char*>{ 1,  9, "cbDo1"      },
        std::tuple<int,int,const char*>{ 1, 31, "cbDo2"      },
        std::tuple<int,int,const char*>{ 1, 53, "cbDo3"      },
        std::tuple<int,int,const char*>{ 1, 12, "cbFr1"      },
        std::tuple<int,int,const char*>{ 1, 34, "cbFr2"      },
        std::tuple<int,int,const char*>{ 1, 56, "cbFr3"      },
        std::tuple<int,int,const char*>{ 1, 14, "cbSa1"      },
        std::tuple<int,int,const char*>{ 1, 36, "cbSa2"      },
        std::tuple<int,int,const char*>{ 1, 58, "cbSa3"      },
        std::tuple<int,int,const char*>{ 1, 16, "cbSo1"      },
        std::tuple<int,int,const char*>{ 1, 38, "cbSo2"      },
        std::tuple<int,int,const char*>{ 1, 60, "cbSo3"      },
    };
    /* varaibales */
    std::vector <std::tuple<int, int,const char*>> var_vec={
        std::tuple<int,int,const char*>{ 0, 30, "vaOffsetSun"},
        std::tuple<int,int,const char*>{ 0, 38, "vaBright"   },
        std::tuple<int,int,const char*>{ 2, 31, "vaVolume"   },
    };
    /* timers */
    std::vector <std::tuple<int, int,const char*>> tim_vec={
        std::tuple<int,int,const char*>{ 0, 28, "tmSerial" },
        std::tuple<int,int,const char*>{ 3,  4, "tmPage3"  },
    };

    std::vector <std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>> callback_vec={
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btSound"    , push, page0_btSoundPushCallback    },
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "tmSerial"   , cmd , page0_tmSerialCmdCallback    }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "tmPage3"    , cmd , page0_tmSerialCmdCallback    }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "page 1"     , cmd , page_save_values             }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "page 2"     , cmd , page_save_values             }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "page 4"     , cmd , page_save_values             }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "page 5"     , cmd , page_save_values             }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "page 6"     , cmd , page_save_values             }, 
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btTimeSync" , push, page2_btTimeSyncPushCallback },
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btSnooze"   , push, page3_btSnoozePushCallback   },
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btLightTest", push, page6_btLightTestPushCallback},
        std::tuple<const char *, typeOfFunc_t, NexTouchEventCb>{ "btReset"    , push, page2_btResetPushCallback    },
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
    /* numbers */
    std::vector<NexNumber *> numbers_vec; 
    /* varaibles */
    std::vector<NexVariable *> vars_vec; 
    /* radio */
    std::vector<NexRadio *> radio_vec; 
    /* ckeckbox */
    std::vector<NexCheckbox *> chkbx_vec; 
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
    std::vector<NexTouch *>    getNexListenList();
    std::vector<NexTimer *>    getNexTimer();
    std::vector<NexVariable *> getNexVariable();
    NexVariable *getNexVariableByName(const char * name_); 
    NexDSButton *getNexButtonByName(const char * name_);
    NexRadio    *getNexRadioByName(const char * name_);
    NexCheckbox *getNexChkbxByName(const char * name_);
    NexNumber   *getNexNumberByName(const char * name_);
    NexPage     *getNexPageByName(const char * name_);
private:
    /* page factory */
    static NexPage*     Create_Pages(int8_t iPage_, const char *name_);
    static NexDSButton* Create_Btns(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexRadio*    Create_Radio(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexCheckbox* Create_Chkbx(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexNumber*   Create_Nmbr(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexVariable* Create_Vars(int8_t iPage_, int8_t iElemet_, const char *name_);
    static NexTimer*    Create_Timers(int8_t iPage_, int8_t iElemet_, const char *name_);
    /* init function */
    void initPages();
    /* add page to pages_vec */
    void addPage(  NexPage *nexPage_     );
    void addBtn(   NexDSButton *nexBtn_  );
    void addRadio( NexRadio *nexRadio_   );
    void addChkbx( NexCheckbox *nexChkbx_);
    void addNmbr(  NexNumber *nexNmbr_   );
    void addVar(   NexVariable *nexVar_  );
    void addTimer( NexTimer *nexTimer_   );
    /* add element to nex_listen_list_vec */
    void addToList(NexTouch *nexTouch_);
    /* attach push-callback functions */
    void attachFunc(const char *, typeOfFunc_t, NexTouchEventCb);
};

#endif