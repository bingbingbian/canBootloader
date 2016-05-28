/******************************************************************************

                  版权所有 (C), 成都天兴仪表股份有限公司

 ******************************************************************************
  文 件 名   : CanNetWorkLayer.h
  版 本 号   : 初稿
  作    者   : Tan Zhihua
  生成日期   : 2013年1月9日
  最近修改   :
  功能描述   : CanNetWorkLayer.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年1月9日
    作    者   : Tan Zhihua
    修改内容   : 创建文件

******************************************************************************/

#ifndef __CANNETWORKLAYER_H__
#define __CANNETWORKLAYER_H__

#include "uds.h"
#include "DrvMSCAN.h"
#include "i15765MsgProc.h"
#include "iso_15765_2_config.h" 

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define CAN_MSG_QUENE_LENGTH            (20)

//#define NWL_UdsMsgTxFlg


#ifndef _FLG_
#define _FLG_

typedef struct 
{
     uint8_t bit7:1;
     uint8_t bit6:1;
     uint8_t bit5:1;
     uint8_t bit4:1;
     uint8_t bit3:1;
     uint8_t bit2:1;
     uint8_t bit1:1;
     uint8_t bit0:1;
}FLG_STRU;

typedef union
{
    uint8_t stats;
    FLG_STRU flg;
}_FLG_UN;

#endif

typedef enum 
{
    NoFault=0,
    TransmitErr,
           
}NWL_ERR_CODE;

#ifndef _ON_OFF_STATUS_
#define _ON_OFF_STATUS_

typedef enum 
{
    TimerOff,
    TimerOn,
}_ON_STATUS_ENUM;

#endif

typedef struct
{
    uint16_t CntData;
    _ON_STATUS_ENUM OnStatus;
}NWL_TIM_STRU;

extern NWL_TIM_STRU NwlTime[NWL_TIM_NUM];


extern void CanQueueAndSemaphoreCreat(void);
extern uint8_t NwFlowCheckAndDeal(UDS_CAN_RX_STRU *pMsg);
extern void NW_ErrCodeProc(uint8_t code);
extern void NwlTimerCnt(void);
extern uint8_t Nw_CanMsgRxProc( void );
extern uint8_t Nw_CanMsgTxProc(void);
extern uint8_t Nw_UdsMsgRxProc(CAN_RX_STRU *pMsg);
extern uint8_t Nw_UdsMsgTxProc(N_UDDATA_REQ_STRU *pMsg);
extern void vCanNetWorkLayerPartTask(void);
//extern uint8_t Nw_UdsWriteTxMsgToQueue(xQueueHandle *Queue,uint8_t Channel);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CANNETWORKLAYER_H__ */
