#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "Driver.h"
#include "Device.h"
#include "AppLow.h"

#include <string.h>
#include <stdbool.h>

/* ------------------------------------------------------------------------------------------------
 *                                      variable
 * ------------------------------------------------------------------------------------------------
 */



/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */

/*IPORT端口定义*/
#define RST         P0_1                    //低电平复位有效
#define FUNCTION    P0_4                    //默认情况下，TCP Server、TCP Client等在连接建立时输出低电平，连接断开时输出高电平               


/****************************************数据帧宏定义******************************************/

/*数据帧包接收情况*/
#define FRAME_RX_START          11          //开始准备接收数据或者空闲
#define FRAME_RX_OK             22          //完整接收了一帧数据命令
#define FRAME_RX_ERR            33          //数据帧有误

/*一帧数据的接收进程*/
#define FRAME_RX_SOP_START      1           //SOP 帧头1
#define FRAME_RX_SOP_CHECK      2           //SOP 帧头2
#define FRAME_RX_LENG           3           //LENG 帧数据长度
#define FRAME_RX_SEQ            4           //SEQ 帧序列
#define FRAME_RX_START_DEV      5           //START_DEV 起始设备，一般为PC
#define FRAME_RX_END_DEV        6           //END_DEV 传输的目的设备类型，基站或者门锁
#define FRAME_RX_DEV_ID         7           //DEV_ID 目的设备ID,基站ID或基站和门锁的ID
#define FRAME_RX_CMD            8           //CMD 传输的命令类型
#define FRAME_RX_DATA           9           //DATA 传输的数据
#define FRAME_RX_LRC            10          //LRC 数据的异或和
#define FRAME_RX_EOP            11          //EOP 帧尾
#define FRAME_RX_END            12          //数据帧如果接收错误则退出数据帧

/*数据帧字节*/
#define SOP                     0xAA

#define SEq                     0x01        

#define START_DEV_PC            0x01        //起始设备为PC
#define START_DEV_BASE          0x02        //起始设备为基站
#define START_DEV_DOOR          0x03        //起始谁被为门锁

#define END_DEV_NONE            0x00
#define END_DEV_PC              0x01        //目的设备为PC   
#define END_DEV_BASE            0x02        //目的设备为基站
#define END_DEV_DOOR            0x03        //目的设备为门锁

#define EOP                     0x0E      










/*数据帧最大长度*/
#define Rx_BUFF                 0xFF
#define Tx_BUFF                 0xFF

/****************************************命令类型宏定义******************************************/

//基站命令类型
#define BASE_CMD_READID         0xA1        //基站ID查看




//基站反馈状态
#define BASE_RESP_OK            0xEE        //操作成功
#define BASE_RESP_SOP_ERR       0x10        //帧头错误
#define BASE_RESP_SEQ_ERR       0x11        //帧序列错误
#define BASE_START_DEV_ERR      0x12        //起始设备识别错误
#define BASE_END_DEV_ERR        0x13        //目标设备识别错误
#define BASE_RESP_LRC_ERR       0x14        //LRC校验错误
#define BASE_RESP_LENG_ERR      0x15        //接收数据帧不完整
#define BASE_RESP_CMD_ERR       0x16        //命令字未知
#define BASE_RESP_EOP_ERR       0x17        //帧尾错误






/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */

//数据帧接收处理
void TcpClient_Init(void);
void TcpClient_Receive(uint8_t Rx_Data);
void TcpClient_Process(uint8_t Rx_Data);
bool TcpClient_IsCmdExist(uint8_t RxCmd);



//main函数轮询数据帧
void TcpClient_IsFullFrame(void);

//基站接收命令处理[数据帧接收错误]
void TcpClient_ErrFrame_Process(void);

//基站接收命令处理[数据帧接收正确]
void TcpClient_BaseCMD_Process(void);

void TcpClient_BaseCMD_ReadID(void);

//发送给PC的数据帧创建处理
void TcpClient_createFrame(uint8_t* frame,uint8_t SEQ, uint8_t FPB_CMD, uint8_t RESP, uint8_t *data,uint8_t dataLength);

/**************************************************************************************************
 */
#endif