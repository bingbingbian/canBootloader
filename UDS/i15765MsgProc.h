/******************************************************************************

                  版权所有 (C), 成都天兴仪表股份有限公司

 ******************************************************************************
  文 件 名   : TPMSProc.h
  版 本 号   : 初稿
  作    者   : xiao bo
  生成日期   : 2014年6月30日
  最近修改   :
  功能描述   : TPMSProc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年6月30日
    作    者   : xiao bo
    修改内容   : 创建文件

******************************************************************************/

#ifndef _UDS_NWL_H_
#define _UDS_NWL_H_

#include "mc9s12hy64.h"
#include "datatype.h"
#include "DrvMSCAN.h"

#include "uds.h"
#include "iso_15765_2_config.h"



#define UDS_RX_MSG_NUM         (2) 

#define UDS_RX_740          (0)
#define UDS_RX_7DF          (1)

#define UDS_TX_MSG_NUM         (1)  

#define UDS_Tx_750          (0)

#define HsCanChannel        (0) 

typedef enum _FS_STATUS
{
    ContinueToSend=0,   /*  继续发送 */
    Wait,               /*  等待发送 */
    OverFlow,           /*  接收溢出 */
}_FS_STATUS_ENUM;   /*流控制状态 */

typedef enum _SEG_SEND
{
    FirstFrame =1,
    ConsecutiveFrame,
}SEG_SEND_ENUM;

typedef struct _UDS_CF_STATUS
{
    uint8_t RxOrTxCmpltFlg; //发送或则接收成功标志
    uint8_t SnCnt;          //帧序号
    uint16_t SnAddData;     //连续帧计数
    uint16_t MsgLthCnt;     //连续帧已接收到消息长度 对每帧的数据字节数累加
}_UDS_CF_STATUS_STRU;     //UDS 连续帧状态




#define NWL_TIM_NUM                     (4)
#define UDS_750_STMIN                   (0)     //发送 STmin
#define UDS_750_NBs                     (1)     //发送方接收流控制帧的等待时间
#define UDS_750_NCs                     (2)     //发送方发送连续帧的间隔时间
#define UDS_740_NCr                     (3)     //接收方接收连续帧的等待时间


#define UDS_BS_750          (0)
#define UDS_BS_7DF          (1)


  //消息流控制状态
extern _UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM]; //连续帧接收计数
extern _UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM]; //连续帧发送计数


extern uint16_t StminCmpData[2];
extern uint8_t NwlTxBsData[2];
extern uint8_t NwlTxBsCnt[2];   //网络层发送 Bs计数 

extern uint8_t UdsMsg750TxOk; 
//extern uint8_t UdsMsg750TxOk;

extern uint8_t g_RxChannel;

extern SEG_SEND_ENUM SegSendFlag;
extern uint8_t UdsMsg750Sending;  //uds 750报文正在发送标志
extern uint8_t g_CFSendFlag; //

extern N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];   //接收 Buffer 网络层到应用层
extern N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];   //发送 Buffer 应用层到网络层
extern _FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM];
//extern _UDS_CF_STATUS_STRU UdsMsgCnt[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM];  //连续帧状态

extern void udsMsgBufInit(void);    // 初始化诊断消息
extern void NWL_CpyData(uint8_t *Dr,uint8_t *Sr,uint8_t MsgLength); // 拷贝诊断消息
extern uint8_t NWL_udsRxMsg(UDS_CAN_RX_STRU *pMsg);   //诊断报文接收处理
extern uint8_t NWL_udsTxMsg(N_UDDATA_REQ_STRU *pMsg);  //诊断报文发送处理
extern uint8_t NWL_RxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg); //接收流控制帧处理
extern uint8_t Nw_UdsSendMsg(uint8_t Channel);
extern void initTxMsgData(uint8_t MsgChannel);  //初始化发送消息
extern void initRxMsgData(uint8_t MsgChannel,uint8_t result);  //初始化接收消息
extern void udsclrMsgBufData(uint8_t *Point,uint8_t MsgLength);
extern uint8_t NWL_TxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg);


#endif
