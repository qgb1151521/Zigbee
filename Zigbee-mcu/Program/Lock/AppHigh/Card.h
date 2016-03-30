#ifndef CARD_H_
#define CARD_H_


/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "AppLow.h"
#include "Driver.h"
#include "Device.h"  
#include <string.h>

//����ͷ�ļ�
#include "Congfig.h"
/* ------------------------------------------------------------------------------------------------
 *                                          Define
 * ------------------------------------------------------------------------------------------------
 */

/********************M1������************************/

/*62����ÿ��Ż�����Ϣ*/
#define BlockSize            16       //һ����16���ֽ�
#define CARD_INFORMATION     62       //M1����62�飬���뿨Ƭ�Ļ�����Ϣ

/******************���Ż�����Ϣ*********************/
/*M1��Ȩ������-CardType(2byte)*/
#define Authorization     0x01        //��Ȩ��
#define UnAuthorizataion  0x02        //ɾȨ��
//#define NullCard          0x03      //�տ�
#define TotalCard         0x04        //�ܿ�
#define FloorCard         0x05        //¥�㿨
//#define OrdinaryCard      0x10      //��ͨ��


/******************M1����������*********************/
#define READ_ID       1               //������ID
#define WRITE_BLOCK   2               //д����������
#define READ_BLOCK    3               //��ȡ��������



/* ------------------------------------------------------------------------------------------------
 *                                          variable
 * ------------------------------------------------------------------------------------------------
 */




/* ------------------------------------------------------------------------------------------------
 *                                          Applications
 * ------------------------------------------------------------------------------------------------
 */
void Card_Init(void);
uint8_t Card_Authorization(uint8_t CardOperType);

/* ------------------------------------------------------------------------------------------------
 *                                          Middle
 * ------------------------------------------------------------------------------------------------
 */
uint8_t Card_ReadID(uint8_t BlockAddr, uint8_t *CardId);
uint8_t Card_WriteBlock(uint8_t BlockAddr, uint8_t *BlockData);
uint8_t Card_ReadBlock(uint8_t BlockAddr, uint8_t *BlockData);

/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
uint8_t Card_Process(uint8_t *CardId, uint8_t BlockAddr, uint8_t *BlockData, uint8_t ProcessType);


#endif