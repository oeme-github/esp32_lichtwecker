#include <MyQueueHandler.h>


MyQueueHandler::MyQueueHandler()
{
}

MyQueueHandler::~MyQueueHandler()
{
}

/**
 * @brief  setQueueMQTT(xQueueHandle hQueue_)
 * @note   setter function for member hQueueMQTT
 * @param  hQueue_: xQueueHandle
 * @retval None
 */
void MyQueueHandler::setQueue(xQueueHandle hQueue_)
{
    this->hQueue = hQueue_;
}

/**
 * @brief  sendToAudioPlayer(String *fileName)
 * @note   sends audiofilename to audioqueue
 * @param  *fileName: 
 * @retval RetCode.msg - text, RetCode.iRet - errorcode
 */
RetCode MyQueueHandler::sendToQueue(std::string msg_)
{
  dbSerialPrintf("sendToQueue(%s)", msg_.c_str());
  /*----------------------------------------------------*/
  /* setup the queue                                    */
  CHAR100 TXmy_struct;
  RetCode retCode;
  /********** LOAD THE DATA ***********/
  strncpy(TXmy_struct, msg_.c_str(), sizeof(TXmy_struct)-1);
  /***** send to the queue ****/
  if( this->hQueue != 0 )
  {
    if( xQueueSend(this->hQueue, (void *)&TXmy_struct, portMAX_DELAY) == pdPASS )
    {
        retCode.first  = 200;
        retCode.second = "msg [" + msg_ + "] sended";
    }
    else
    {
        retCode.first  = 500;
        retCode.second = "ERROR: sending";
    }
  }
  else
  {
        retCode.first  = 510;
        retCode.second = "ERROR: no queue";
  }
  dbSerialPrintf( "%i - %s", retCode.first, retCode.second.c_str());
  return retCode;
}

/**
 * @brief  readFromQueue()
 * @note   read next idem from queue
 * @retval RetCode: std::pair<int, std::string>
 */
RetCode MyQueueHandler::readFromQueue()
{
    /* -------------------------------------------------- */ 
    RetCode retCode;
    CHAR100 Rptrtostruct;
    /*-------------------------------------------------------*/
    /* setup the queue                                       */
    if (xQueueReceive(this->hQueue, &Rptrtostruct, portMAX_DELAY) == pdPASS)
    {
        retCode.first  = 0;
        retCode.second = Rptrtostruct;
    }
    else
    {
        retCode.first  = -1;
        retCode.second = "";
    }
    dbSerialPrintf( "%i - %s", retCode.first, retCode.second.c_str());
    return retCode;
}
