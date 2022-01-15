#include <NextionDisplay.h>


/**
 * @brief Construct a new Nextion Disp:: Nextion Disp object
 * 
 * @param iSerialSpeed_ 
 */
NextionDisplay::NextionDisplay(int iSerialSpeed_)
{
    dbSerialPrintln("constructor NextionDisplay");
    ESPNexUpload nextion(iSerialSpeed_);
    nexUplSrv = &nextion;
}

/**
 * @brief Construct a new Nextion Disp:: Nextion Disp object 
 * 
 */
NextionDisplay::NextionDisplay(/* args */)
{
    NextionDisplay(115200); 
}

/**
 * @brief Destroy the Nextion Disp:: Nextion Disp object
 * 
 */
NextionDisplay::~NextionDisplay()
{
}

void NextionDisplay::InitDisplay()
{
    dbSerialPrintln("InitDisplay()...");
    boolean bRet=false;
    for(int i=0; i<=5; i++)
    {
        bRet = nexInit();
        if( bRet )
        {
            break;
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
    if(bRet)
    {
        dbSerialPrintln("display started...");
        dbSerialPrintln("init pages...");
        initPages();
        dbSerialPrintln("add callback functions...");
        /** 
         * add Nextion callbacks
         */
        for(auto & elem : pages.callback_vec )
        {
            attachFunc( std::get<0>(elem), std::get<1>(elem), std::get<2>(elem) );
        }
    } 
    else
    {
        dbSerialPrintln("ERROR: could not start display!!!!");
        dbSerialPrintln("restart.");
        vTaskDelay(2000/portTICK_PERIOD_MS);
        ESP.restart();
    }
}

/**
 * @brief Page-Factory
 * 
 * @param iPage_ 
 * @param name_ 
 * @return NexPage* 
 */
NexPage* NextionDisplay::Create_Pages(int8_t iPage_, const char *name_)
{
    dbSerialPrintln( (String("Create_Pages(") + iPage_ + ", " + String(name_) + String(")")).c_str() );
    return new NexPage( iPage_, 0, name_); 
}

/**
 * @brief button factory
 * 
 * @param iPage_ 
 * @param iElemet_ 
 * @param name_ 
 * @return NexDSButton* 
 */
NexDSButton* NextionDisplay::Create_Btns(int8_t iPage_, int8_t iElemet_, const char *name_)
{
    dbSerialPrintln( (String("Create_Btns(") + iPage_ + ", " + iElemet_ + ", " + String(name_) + String(")")).c_str() );
    return new NexDSButton( iPage_, iElemet_, name_); 
}

/**
 * @brief variable factory
 * 
 * @param iPage_ 
 * @param iElemet_ 
 * @param name_ 
 * @return NexVariable* 
 */
NexVariable* NextionDisplay::Create_Vars(int8_t iPage_, int8_t iElemet_, const char *name_)
{
    dbSerialPrintln( (String("Create_Vars(") + iPage_ + ", " + iElemet_ + ", " + String(name_) + String(")")).c_str() );
    return new NexVariable( iPage_, iElemet_, name_); 
}

/**
 * @brief timer factory
 * 
 * @param iPage_ 
 * @param iElemet_ 
 * @param name_ 
 * @return NexTimer* 
 */
NexTimer* NextionDisplay::Create_Timers(int8_t iPage_, int8_t iElemet_, const char *name_)
{
    dbSerialPrintln( (String("Create_Timers(") + iPage_ + ", " + iElemet_ + ", " + String(name_) + String(")")).c_str() );
    return new NexTimer( iPage_, iElemet_, name_); 
}

/**
 * @brief initializes pages from page_vec
 * 
 */
void NextionDisplay::initPages(){
    dbSerialPrintln("initPages()");
    /* -------------------------------------------------- */
    /* create pages                                       */
    for(auto & elem : pages.page_vec)
    {        
        addPage( Create_Pages(elem.first, elem.second) );
    }
    /* -------------------------------------------------- */
    /* buttons                                            */
    for(auto & elem : pages.btn_vec )
    {
        addBtn( Create_Btns(std::get<0>(elem), std::get<1>(elem), std::get<2>(elem)) );
    }
    /* -------------------------------------------------- */
    /* varaibles                                          */
    for(auto & elem : pages.var_vec )
    {
        addVar( Create_Vars(std::get<0>(elem), std::get<1>(elem), std::get<2>(elem)) );
    }
    /* -------------------------------------------------- */
    /* timers                                             */
    for(auto & elem : pages.tim_vec )
    {
        addTimer( Create_Timers(std::get<0>(elem), std::get<1>(elem), std::get<2>(elem)) );
    }
    /* -------------------------------------------------- */
    /* add NULL                                           */
    /* nex_listen_list_vec.push_back(NULL); */

    for( int i=0; i < nex_listen_list_vec.size(); i++ )
    {
        nex_listen_list_vec[i]->printObjInfo();
    }
}

/**
 * @brief add page to pages_vec and nextion listen list
 * 
 * @param nexPage_ 
 */
void NextionDisplay::addPage(NexPage *nexPage_)
{
    pages_vec.push_back( nexPage_ );
    addToList( (NexTouch *)nexPage_ );
}

/**
 * @brief add button to buttons_vec and nextion listen list
 * 
 * @param nexButton_ 
 */
void NextionDisplay::addBtn(NexDSButton *nexBtn_)
{
    buttons_vec.push_back( nexBtn_ );
    addToList( (NexTouch *)nexBtn_ );
}

/**
 * @brief add variable to vars_vec and nextion listen list
 * 
 * @param nexVar_ 
 */
void NextionDisplay::addVar(NexVariable *nexVar_)
{
    vars_vec.push_back( nexVar_ );
    addToList( (NexTouch *)nexVar_ );
}

/**
 * @brief add timer to timers_vec and nextion listen list
 * 
 * @param nexTimer_ 
 */
void NextionDisplay::addTimer(NexTimer *nexTimer_)
{
    timers_vec.push_back( nexTimer_ );
    addToList( (NexTouch *)nexTimer_ );
}

/**
 * @brief add elemt to nextion listen list 
 * 
 * @param nexTouch_ 
 */
void NextionDisplay::addToList(NexTouch *nexTouch_)
{
    nex_listen_list_vec.push_back(nexTouch_);
}

/**
 * @brief attach callback functions to elements (NextObj)
 * 
 * @param objName_ 
 * @param typeOfFunc_ 
 * @param callBackFunc_ 
 */
void NextionDisplay::attachFunc(const char * objName_, typeOfFunc_t typeOfFunc_, NexTouchEventCb callBackFunc_)
{
    dbSerialPrint( ("attachFunc(" + String(objName_) + ", " ).c_str() ); 
    switch (typeOfFunc_)
    {
    case 0:
        dbSerialPrint("push, ");
        break;
    case 1:
        dbSerialPrint("pop, ");
        break;
    case 2:
        dbSerialPrint("cmd, ");
        break;    
    default:
        break;
    }
    dbSerialPrintln( ")");

    /* find object in callback_vec*/
    for(auto & elem : nex_listen_list_vec )
    {
        if( elem->getName() == objName_ )
        {
            dbSerialPrintln("");
            switch (typeOfFunc_)
            {
            case push:
                /* code */
                elem->attachPush(callBackFunc_);
                break;
            case pop:
                /* code */
                elem->attachPop(callBackFunc_);
                break;
            case cmd:
                /* code */
                elem->attachCmd(callBackFunc_);
                break;
            default:
                break;
            }
        }
    }    
}

/**
 * @brief getter function for nex_listen_list_vec
 * 
 */
std::vector<NexTouch *> NextionDisplay::getNexListenList()
{
    return nex_listen_list_vec;
}

std::vector<NexTimer *> NextionDisplay::getNexTimer()
{
    return timers_vec;    
}

std::vector<NexVariable *> NextionDisplay::getNexVariable()
{
    return this->vars_vec;    
}

NexVariable * NextionDisplay::getNexVariableByName(const char * name_)
{
    /* find object in  vars_vec*/
    for(auto & elem : this->vars_vec )
    {
        if( elem->getName() == name_ )
            return elem;
    }
    return NULL;
}

NexDSButton *NextionDisplay::getNexButtonByName(const char * name_)
{
    /* find object in  vars_vec*/
    for(auto & elem : this->buttons_vec )
    {
        if( elem->getName() == name_ )
            return elem;
    }
    return NULL;
}
