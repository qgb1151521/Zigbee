#ifndef MAIN_H_
#define MAIN_H_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include<ioCC2530.h>
#include "Driver.h"         //驱动层
#include "Device.h"         //设备层
#include "AppHigh.h"        //顶层应用层
#include "AppLow.h"         //底层应用层
#include <string.h>



/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */

/*钥匙开门状态标识*/
#define OPEN_YES            1     //钥匙开门
#define OPEN_NO             2     //钥匙未开门


/*钥匙端口定义*/
#define KEY                 P0_1  //检测钥匙开门
  #define TURNON            0     //转过来
  #define TURNOFF           1     //转回去

/*运行模式*/
#define BatteryDetection    1     //电池电压检测
#define ReadCardAction      2     //刷卡授权和开门
#define KeyAction           3     //机械开门
#define RxBaseStation       4     //接收基站命令
#define TxBaseStation       5     //发送给基站数据
#define SysIntoSleep        6     //系统进入睡眠状态





/* ------------------------------------------------------------------------------------------------
 *                                          单文件全局变量
 * ------------------------------------------------------------------------------------------------
 */



uint8_t IsKeyOpen;      


/* ------------------------------------------------------------------------------------------------
 *                                          多文件全局变量
 * ------------------------------------------------------------------------------------------------
 */










#endif