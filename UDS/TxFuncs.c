/******************************************************************************

                  ��Ȩ���� (C), �ɶ������Ǳ�ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : TxFuncs.c
  �� �� ��   : ����
  ��    ��   : Tan Zhihua
  ��������   : 2013��1��3��
  ����޸�   :
  ��������   : ���ͺ���ģ��
  �����б�   :
              i15765_tx_app
  �޸���ʷ   :
  1.��    ��   : 2013��1��3��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ļ�

******************************************************************************/
#include "uds.h"
#include "CanNetWorkLayer.h"
#include "TxFuncs.h"

N_UDDATA_REQ_STRU g_AppTxReq;   //Ӧ�ò�����ԭ��
uint8_t g_AppBuf[MAX_UDS_TX_BUF_BYTES];   //Ӧ�ò㷢�͵��� buffer
uint8_t g_AppTxReqflg;          //Ӧ�ò�����ԭ�﷢�ͱ�־

/*****************************************************************************
 �� �� ��  : i15765_tx_app
 ��������  : 15765���ķ��ʹ���
 �������  : i15765_t*msg                
             I15765_TX_STATUS *txStatus  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void i15765_tx_app( i15765_t *msg, I15765_TX_STATUS *txStatus)
{   
    uint16_t cycle;
    
    for ( cycle = 0 ; cycle < MAX_UDS_TX_BUF_BYTES ; cycle++ )
    {
        g_AppTxReq.buf[cycle]=0x00;
    }
    
    g_AppTxReq.sa=msg->sa;
    g_AppTxReq.ta=msg->ta;
    g_AppTxReq.ae=msg->ae;
    g_AppTxReq.tat=msg->tat;
    g_AppTxReq.pri=msg->pri;
    g_AppTxReq.buf_len=msg->buf_len;
    for ( cycle = 0 ; cycle < msg->buf_len; cycle++ )
    {
        g_AppTxReq.buf[cycle]=msg->buf[cycle];
    }
    
    g_AppTxReqflg = 1; //��λӦ�ò�����ԭ�﷢�ͱ�־ ֪ͨ�����
    
    *txStatus=I15765_SENT;//�ô�Ӧ���ж� ��������
    
}


