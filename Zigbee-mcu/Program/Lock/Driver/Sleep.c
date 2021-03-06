#define SLEEP_C_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Sleep.h"

/**************************************************************************************************
 * @fn          Sleep_INT_Init
 * @brief       P1.6端口睡眠中断设置初始化(中断暂时关闭)
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Sleep_INT_Init(void)
{
  
  P1SEL &= ~0x40;   //设置P1_6为通用I/O口
  P1DIR &= ~0x40;   //设置P1_6为输入
  P1INP &= ~0x40;   //设置P1_6为上拉，默认是高电平
  
  //IEN2  |=  0x10;   //端口1中断使能
  //P1IEN |=  0x40;   //使能P1_6中断
  //PICTL |=  0x04;   //输入的下降沿触发中断
  //P1IFG &=  ~0x40;  //P1.6中断标志清0
}



/**************************************************************************************************
 * @fn          Sleep_Init
 * @brief       系统睡眠初始化
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Sleep_Init(void)
{
  ST2   = 0x00;
  ST1   = 0x00;
  ST0   = 0x00;   //32KhZ下计数
  
  STIE  = 1;      //睡眠定时器中断使能
  STIF  = 0;      //睡眠定时器中断标志，0：无中断未决 1：中断未决
}


/**************************************************************************************************
 * @fn          Sleep_SetTime
 * @brief       系统睡眠时间设定
 * @param       无
 * @return      无
 **************************************************************************************************
 */



void Sleep_SetTime(uint8_t Sleep_Sec)
{
  uint32_t SleepTime = 0;
 
  SleepTime |=  ST0;
  SleepTime |=  (uint32_t)ST1 << 8;
  SleepTime |=  (uint32_t)ST2 << 16;
  SleepTime +=  (uint32_t)Sleep_Sec * (uint32_t)32768;
  ST2        =  (uint8_t)(SleepTime >> 16);
  ST1        =  (uint8_t)(SleepTime >> 8);
  ST0        =  (uint8_t) SleepTime;
  
//  count = sizeof(uint64_t);         //4
//  count = sizeof(uint32_t);         //2
//  count = sizeof(uint16_t);         //2
//  count = sizeof(uint8_t);          //1
  
}


/**************************************************************************************************
 * @fn          Sleep_Set
 * @brief       系统睡眠时间设定1.5S
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Sleep_Set(void)
{
  uint32_t SleepTime = 0;
  
  SleepTime |=  ST0;
  SleepTime |=  (uint32_t)ST1 << 8;
  SleepTime |=  (uint32_t)ST2<< 16;
  SleepTime +=  (uint32_t)36044;    //1.1*32768 1.1s休眠一次
  ST2        =  (uint8_t)(SleepTime >> 16);
  ST1        =  (uint8_t)(SleepTime >> 8);
  ST0        =  (uint8_t) SleepTime;
}




/**************************************************************************************************
 * @fn          Sleep_SetMode
 * @brief       系统工作模式设定
 * @param       无
 * @return      无
 **************************************************************************************************
 */
void Sleep_SetMode(uint8_t Sleep_Mode)
{
  if(Sleep_Mode < 4)
  {
    SLEEPCMD &= 0xFC;
    SLEEPCMD |= Sleep_Mode;   //设置系统睡眠模式
    PCON      = 1;            //进入睡眠模式，通过中断唤醒
  }
  else
  {
    PCON      = 0;            //唤醒睡眠模式
  }
}

///**************************************************************************************************
// * @fn          ST_ISR
// * @brief       睡眠定时器中断
// * @param       无
// * @return      无
// **************************************************************************************************
// */
//#pragma vector = ST_VECTOR
//__interrupt void ST_ISR(void)
//{
//  STIF = 0x00;                //清除中断标志
//  Sleep_SetMode(SLEEP_OFF);   //进入正常工作模式
//}