#define CARD_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Card.h"

/* ------------------------------------------------------------------------------------------------
 *                                          Local Variable
 * ------------------------------------------------------------------------------------------------
 */
uint8_t  SectorKeyA[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};                       //A扇区秘钥

/* ------------------------------------------------------------------------------------------------
 *                                          Applications
 * ------------------------------------------------------------------------------------------------
 */

/**************************************************************************************************
 * @fn          Card_AccountInit
 * @brief       授权卡初始化，只做一次
 * @param       无       
 * @return      无
 **************************************************************************************************
 */
void Card_Init(void)
{
  /*
    CARD_INFORMATION M1的62区块 16byte数据
    |-------------------------------------------------------|
    |         | Byte0-1  | Byte2-5  |           |  
    | BLOCK62 |---------------------------------------------|
    |         | CardType | DoorId   |           |    
    |-------------------------------------------------------|
  
    CardType: 0x01 0x01 授权卡标识
              0x02 0x02 删权卡标识
              0x03 0x03 空卡标识
              0x04 0x04 总卡标识(所有门可开)
  
    DoorId  : Byte2 - DOOR_COMPANY   除了普通卡、空卡以外都需要这些基础信息
              Byte3 - DOOR_BUILD     
              Byte4 - DOOR_FLOOR
              //Byte5 - DOOR_ID   
  */
  
  
  /*0.授权卡,删权卡,总卡卡号数据....*/
  uint8_t Status                =  MFRC522_ERR;
  uint8_t AuthCard[BlockSize]   = {Authorization,Authorization,DOOR_COMPANY,DOOR_BUILD};         //授权卡
  uint8_t UnAuthCard[BlockSize] = {UnAuthorizataion,UnAuthorizataion,DOOR_COMPANY,DOOR_BUILD};   //删权卡
  uint8_t TotCard[BlockSize]    = {TotalCard,TotalCard,DOOR_COMPANY,DOOR_BUILD};                                        //总卡
  uint8_t ReadData[BlockSize]  = {0x00};                                        //读M1卡扇区数据                       

  /*1.授权卡*/  
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);                           //连续读写下需要打开Card_ReadBlock的Reset函数
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,AuthCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
  
  /*2.删权卡*/
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,UnAuthCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
  
  
  /*3.总卡*/
  Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
  if(Status == MFRC522_OK)
  {
    Status = Card_WriteBlock(CARD_INFORMATION,TotCard);
    if(Status == MFRC522_OK)
    {
      Warn_One_Buzzer();
    }
  }
 
}



/**************************************************************************************************
 * @fn          Card_Authorization
 * @brief       授权操作，刷授权卡之后3S内寻卡授权操作
 * @param       CardType    ->  看授权类型，授权卡还是删权卡     
 * @return      Status      ->  MFRC522_OK  正确  
                                MFRC522_ERR 错误
 **************************************************************************************************
 */
uint8_t Card_Authorization(uint8_t CardOperType)
{
  uint8_t Status               = MFRC522_ERR;                                                                       
  uint8_t ReadData[BlockSize]  = {0x00};                                        //读M1卡扇区数据 
  uint8_t CardCount            = 0;                                             //刷卡次数
  uint8_t DoorId[4]            = {0x00};                                        //门锁ID
  uint8_t DataResult           = DATA_ERR;                                      //数据处理结果
  
  Timer_Int_Enable();                                                           //允许定时中断
  T1_3S_Flag = TIMESTART;                 
  
  do
  {
    /*1.读取卡号区块数据*/
    Status = Card_ReadBlock(CARD_INFORMATION,ReadData);
    if(Status == MFRC522_OK)                
    {
      /*2.授权操作*/
      if(CardOperType == Authorization)
      {
        if(((ReadData[0] == Authorization)    && (ReadData[1] == Authorization))    ||
           ((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion)) ||
           ((ReadData[0] == TotalCard)        && (ReadData[1] == TotalCard)))
        {
          Status = MFRC522_ERR;                                                 //如果是授权卡、删权卡、总卡则报错
        }
        
        else                                                                    //普通卡授权处理                             
        {
          DataResult = Data_CommonCard_Auth(ReadData+12);                       //普通卡存储
          if(DataResult == DATA_OK)
          {
            Status =  MFRC522_OK; 
          }
          else if(DataResult == DATA_ERR)                                       //该授权卡已经授权或者存储失败
          {
            Status = MFRC522_ERR;
          }
          else if(DataResult == DATA_FULL)
          {
            T1_3S_Flag = TIMEEND;                                               //普通卡列表已满警告退出
            Warn_Card_Full();
          }
        }
      }
 
      /*3.删权操作*/
      else if(CardOperType == UnAuthorizataion)
      {
        Data_DoorID_Read(DoorId);                                               //门锁ID读取
        if((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion) &&
           (ReadData[2] == DoorId[0])        && (ReadData[3] == DoorId[1]))     //删权,必须是该公司该楼层的删权卡有效
        {
          CardCount ++; 
          T1_3S_Flag = TIMEEND;
          Warn_One_Buzzer();
          T1_3S_Flag = TIMESTART;                                                  
          LED_ON();                                                             //提示刷删权卡
        }
        
        else if(((ReadData[0] == Authorization)    && (ReadData[1] == Authorization))    ||
                ((ReadData[0] == UnAuthorizataion) && (ReadData[1] == UnAuthorizataion)) ||
                ((ReadData[0] == TotalCard)        && (ReadData[1] == TotalCard)))
        {
          Status = MFRC522_ERR;                                                 //如果是授权卡、总卡、或者其他楼层的删权卡
        }
        
        else                                                                    //删除普通卡权限
        {
          CardCount = 0;        
          DataResult = Data_CommonCard_UnAuth(ReadData+12);
          
          if(DataResult == DATA_OK)
          {
            Status =  MFRC522_OK; 
          }
          else if(DataResult == DATA_ERR)
          {
            Status = MFRC522_ERR;
          }
        }
        
         
        if(CardCount >= 5)                                                      //普通卡数据初始化
        {                                                                                   
          T1_3S_Flag = TIMEEND;                                                 //跳出while
          Data_Door_Init();                                                     //门锁初始化，清空普通卡列表，还有清空记录等后续操作
          LED_OFF();
          Warn_Door_Init();
          LED_ON();
        }
      }
      
      /*4.操作成功*/
      if((Status == MFRC522_OK) && (DataResult != DATA_FULL) && (CardCount == 0))
      {    
        T1_3S_Flag = TIMEEND;
        Warn_Card_Success();                                                    //保证3S计数值清0
        T1_3S_Flag = TIMESTART;                                                 //可以重复授权 
        
//        DataResult = AT24C256_isNull();                                    //测试用
//        if(AT24C256_Result == AT24C256_NULL)                                    //类似于初始化门锁了
//        {
//          Buzzer_CardInit();
//        }
      }
      
      /*5.操作失败*/
      if((Status == MFRC522_ERR) && (DataResult != DATA_FULL) && (CardCount == 0))
      {
        T1_3S_Flag = TIMEEND;
        Warn_Card_Fail();
        LED_ON();
        T1_3S_Flag = TIMESTART;                                                 //可以重复授权 
          
//        AT24C256_Result = AT24C256_isNull();                                    //测试用
//        if(AT24C256_Result == AT24C256_NULL)                                    //类似于初始化门锁了
//        {
//          Buzzer_CardInit();
//        }
      }
      
      /*6.清除数据*/
      Status = MFRC522_ERR;
      DataResult = DATA_ERR;
      memset(ReadData,0,BlockSize);
      memset(DoorId,0,4);
    }
    
    //Wdt_FeetDog();                                                              //必须喂狗，这里并不是用于判断程序跑飞，而是因为程序运行时间已经超了1S
   
  }while(T1_3S_Flag == TIMESTART);                                              //3S超时跳出
  
  
  T1_3S_Flag = TIMEINIT;    
  Timer_Int_Disable();

  return Status;
}








/* ------------------------------------------------------------------------------------------------
 *                                          Middle
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          Card_ReadID
 * @brief       读取卡号
 * @param       BlockAddr   ->  区块地址
                BlockData   ->  区块数据           
 * @return      Status      ->  MFRC522_OK  正确  
                                MFRC522_ERR 错误
 **************************************************************************************************
 */
uint8_t Card_ReadID(uint8_t BlockAddr, uint8_t *CardId)
{
  uint8_t Status;
  uint8_t Block[BlockSize]  = {0x00};
  
  MFRC522_AntennaOn();                                                          //实现连续的读卡操作和读写操作必须开关天线
  //memcpy(WriteBlock,BlockData,6);                                             //主要赋值卡的类型，授权卡或者普通卡等
  Status = Card_Process(CardId,BlockAddr,Block,READ_ID);                        //读写模式下其实根本不用传输卡号ID     
  MFRC522_AntennaOff();
  Delay_Ms(10);

  return Status;
}



/**************************************************************************************************
 * @fn          Card_WriteBlock
 * @brief       写卡扇区的数据
 * @param       BlockAddr   ->  区块地址
                BlockData   ->  区块数据           
 * @return      Status      ->  MFRC522_OK  正确  
                                MFRC522_ERR 错误
 **************************************************************************************************
 */
uint8_t Card_WriteBlock(uint8_t BlockAddr, uint8_t *BlockData)
{
  uint8_t CardId[6]       = {0x00};
  uint8_t WriteBlock[BlockSize]  = {0x00};
  uint8_t ReadBlock[BlockSize] = {0x00};   
  uint8_t Status;
  
  /*1.写入卡号数据*/
  MFRC522_Reset();                                                              //写卡很少用，所以就Reset吧   
  MFRC522_AntennaOn();                                                          //实现连续的读卡操作和读写操作必须开关天线
  memcpy(WriteBlock,BlockData,BlockSize);                                       //赋值卡号基本信息                 
  Status = Card_Process(CardId,BlockAddr,WriteBlock,WRITE_BLOCK);               //读写模式下其实根本不用传输卡号ID
  MFRC522_AntennaOff();
  Delay_Ms(10);
  if(Status == MFRC522_ERR)                                                     //如果不授权的情况下应该直接返回了
  {
    return MFRC522_ERR;
  }
  
  /*2.如果写入成功，则读取写入的数据*/
  MFRC522_Reset();                                                              //要实现连续的读写操作必须重新Reset 
  MFRC522_AntennaOn();  
  Status = Card_ReadBlock(CARD_INFORMATION,ReadBlock);
  if(Status == MFRC522_OK)
  {
    if(strncmp((const char*)ReadBlock,(const char*)BlockData,4)!= 0)
    {
      Status = MFRC522_ERR;                                                     //写入的和读取的不一致
    }
    
    else
    {
      Status = MFRC522_OK;                                                      //写入的和读取的一致
      return Status;
    }
  }
  
  /*3.如果写入读取不一致，则重新写入*/
  if(Status == MFRC522_ERR)
  { 
    MFRC522_Reset();                                                            //写卡很少用，所以就Reset吧   
    MFRC522_AntennaOn();                                                        //实现连续的读卡操作和读写操作必须开关天线              
    Status = Card_Process(CardId,BlockAddr,WriteBlock,WRITE_BLOCK);             //读写模式下其实根本不用传输卡号ID
    MFRC522_AntennaOff();
    Delay_Ms(10);
    if(Status == MFRC522_ERR)                                                   //如果不授权的情况下应该直接返回了
    {
      return MFRC522_ERR;
    }
    
    /*重新读取写入的数据*/
    MFRC522_Reset();                                                            //要实现连续的读写操作必须重新Reset 
    MFRC522_AntennaOn();  
    Status = Card_ReadBlock(CARD_INFORMATION,ReadBlock);
    if(Status == MFRC522_OK)
    {
      if(strncmp((const char*)ReadBlock,(const char*)BlockData,4)!= 0)          //写入的和读取的不一致
      {
        Status = MFRC522_ERR;
      }
        
      else
      {
        Status = MFRC522_OK;                                                    //写入的和读取的一致
      }
    } 
  }
  
  return Status;
}


/**************************************************************************************************
 * @fn          Card_ReadBlock
 * @brief       读卡扇区的数据
 * @param       BlockAddr   ->  区块地址
                BlockData   ->  区块数据           
 * @return      Status      ->  MFRC522_OK  正确  
                                MFRC522_ERR 错误
 **************************************************************************************************
 */
uint8_t Card_ReadBlock(uint8_t BlockAddr, uint8_t *BlockData)
{
  uint8_t CardId[6]       = {0x00};
  uint8_t ReadBlock[BlockSize] = {0x00};
  uint8_t Status;
  
  //MFRC522_Reset();                                                              //要实现连续的读写操作必须重新Reset，这里可能需要注意
  MFRC522_AntennaOn();  
  
  Status = Card_Process(CardId,BlockAddr,ReadBlock,READ_BLOCK);
  if(Status == MFRC522_ERR)
  {
    return MFRC522_ERR;
  }
  memcpy(BlockData,ReadBlock,BlockSize);                                        //卡号基本信息
  memcpy(BlockData+12,CardId,BlockSize-12);                                     //卡号ID
 
  MFRC522_AntennaOff();
  Delay_Ms(10);
  
  return Status;
}


/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
/**************************************************************************************************
 * @fn          Card_Process
 * @brief       读卡处理
 * @param       CardId      ->  读取的卡号ID
                BlockAddr   ->  区块地址
                BlockData   ->  区块数据
                ProcessType ->  操作类型  
                                WRITE_BLOCK 写区块数据
                                READ_BLOCK  读区块数据
 * @return      Status      ->  MFRC522_OK  正确  
                                MFRC522_ERR 错误
 **************************************************************************************************
 */
uint8_t Card_Process(uint8_t *CardId, uint8_t BlockAddr, uint8_t *BlockData, uint8_t ProcessType)
{
  
  
  uint8_t Status;
  uint8_t CardData[BlockSize] = {0x00};
  //uint16_t CardType;

  /*1.寻卡,返回卡号类型*/
  Status = MFRC522_RequestCard(PICC_REQIDL,CardData);
  if(Status == MFRC522_OK)
  {
    //CardType = (CardData[0]<<8)| CardData[1];                                 //Mifare_One(S50) 校园卡
  }
  
  /*2.防冲撞，返回卡号序列*/
  Status = MFRC522_Anticoll(CardData);
  if(Status == MFRC522_OK)
  {
    memcpy(CardId,CardData,5);   
  }
  
  if(ProcessType == WRITE_BLOCK)
  {
    /*3.选卡,返回卡容量*/ 
    MFRC522_SelectTag(CardId);                                                  //Size = 0x08
    
    /*4.写数据*/
    Status = MFRC522_Auth(PICC_AUTHENT1A,BlockAddr,SectorKeyA,CardId);
    if(Status == MFRC522_OK)
    {
      Status = MFRC522_WriteToBlock(BlockAddr,BlockData);
    }
  }
 
  else if(ProcessType == READ_BLOCK)
  {
    /*3.选卡,返回卡容量*/ 
    MFRC522_SelectTag(CardId);                
    
    /*4.读数据*/
    Status = MFRC522_Auth(PICC_AUTHENT1A,BlockAddr,SectorKeyA,CardId);
    if(Status == MFRC522_OK)
    {
      Status = MFRC522_ReadFromBlock(BlockAddr,CardData);
      if(Status == MFRC522_OK)
      {
        memcpy(BlockData,CardData,BlockSize);                                   //如果读正确
      }
    }  
  }
  
  return Status;
}




