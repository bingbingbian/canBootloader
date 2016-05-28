/******************************************************************************

                  ��Ȩ���� (C), �ɶ������Ǳ�ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : TPMSProc.h
  �� �� ��   : ����
  ��    ��   : xiao bo
  ��������   : 2014��6��30��
  ����޸�   :
  ��������   : TPMSProc.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2014��6��30��
    ��    ��   : xiao bo
    �޸�����   : �����ļ�

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
    ContinueToSend=0,   /*  �������� */
    Wait,               /*  �ȴ����� */
    OverFlow,           /*  ������� */
}_FS_STATUS_ENUM;   /*������״̬ */

typedef enum _SEG_SEND
{
    FirstFrame =1,
    ConsecutiveFrame,
}SEG_SEND_ENUM;

typedef struct _UDS_CF_STATUS
{
    uint8_t RxOrTxCmpltFlg; //���ͻ�����ճɹ���־
    uint8_t SnCnt;          //֡���
    uint16_t SnAddData;     //����֡����
    uint16_t MsgLthCnt;     //����֡�ѽ��յ���Ϣ���� ��ÿ֡�������ֽ����ۼ�
}_UDS_CF_STATUS_STRU;     //UDS ����֡״̬




#define NWL_TIM_NUM                     (4)
#define UDS_750_STMIN                   (0)     //���� STmin
#define UDS_750_NBs                     (1)     //���ͷ�����������֡�ĵȴ�ʱ��
#define UDS_750_NCs                     (2)     //���ͷ���������֡�ļ��ʱ��
#define UDS_740_NCr                     (3)     //���շ���������֡�ĵȴ�ʱ��


#define UDS_BS_750          (0)
#define UDS_BS_7DF          (1)


  //��Ϣ������״̬
extern _UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM]; //����֡���ռ���
extern _UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM]; //����֡���ͼ���


extern uint16_t StminCmpData[2];
extern uint8_t NwlTxBsData[2];
extern uint8_t NwlTxBsCnt[2];   //����㷢�� Bs���� 

extern uint8_t UdsMsg750TxOk; 
//extern uint8_t UdsMsg750TxOk;

extern uint8_t g_RxChannel;

extern SEG_SEND_ENUM SegSendFlag;
extern uint8_t UdsMsg750Sending;  //uds 750�������ڷ��ͱ�־
extern uint8_t g_CFSendFlag; //

extern N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];   //���� Buffer ����㵽Ӧ�ò�
extern N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];   //���� Buffer Ӧ�ò㵽�����
extern _FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM];
//extern _UDS_CF_STATUS_STRU UdsMsgCnt[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM];  //����֡״̬

extern void udsMsgBufInit(void);    // ��ʼ�������Ϣ
extern void NWL_CpyData(uint8_t *Dr,uint8_t *Sr,uint8_t MsgLength); // ���������Ϣ
extern uint8_t NWL_udsRxMsg(UDS_CAN_RX_STRU *pMsg);   //��ϱ��Ľ��մ���
extern uint8_t NWL_udsTxMsg(N_UDDATA_REQ_STRU *pMsg);  //��ϱ��ķ��ʹ���
extern uint8_t NWL_RxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg); //����������֡����
extern uint8_t Nw_UdsSendMsg(uint8_t Channel);
extern void initTxMsgData(uint8_t MsgChannel);  //��ʼ��������Ϣ
extern void initRxMsgData(uint8_t MsgChannel,uint8_t result);  //��ʼ��������Ϣ
extern void udsclrMsgBufData(uint8_t *Point,uint8_t MsgLength);
extern uint8_t NWL_TxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg);


#endif
