
#include "CanNetWorkLayer.h"
#include "DrvMSCAN.h"
#include "TxFuncs.h"
#include "i15765MsgProc.h"
#include "UdsApp.h"



uint8_t g_UdsNetRstFlag = 1;        /* UDS ����㸴λ��־ */
NWL_TIM_STRU NwlTime[NWL_TIM_NUM];  /* ����㶨ʱ�� */


/*****************************************************************************
 �� �� ��  : Nw_CanMsgRxProc
 ��������  : �����can���Ľ��մ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t Nw_CanMsgRxProc( void )
{
    uint8_t res = TRUE;    
    if(UDS_MSG.Flag == 1) //�ӵ�����ϱ���
    {   
        UDS_MSG.Flag = 0;
        res=NWL_udsRxMsg(&(UDS_MSG)); //��ϱ��Ĵ���
    }
    return res;
}
/*****************************************************************************
 �� �� ��  : NwFlowCheckAndDeal
 ��������  : ������֡���ʹ���
 �������  : UDS_CAN_RX_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��22�����ڶ�
    ��    ��   : fsfs
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NwFlowCheckAndDeal(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t res = TRUE;
    if((pMsg->Data[0]&0xF0) == 0x30)   //PCI ������֡
    {
        pMsg->Flag = 0;
        res=NWL_udsRxMsg(pMsg); //��ϱ��Ĵ���
    }
    return res;
}
/*****************************************************************************
 �� �� ��  : Nw_CanMsgTxProc
 ��������  : �����CAN���ķ��ʹ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��3��������
    ��    ��   : xiaobo
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t Nw_CanMsgTxProc(void)
{
    uint8_t cnt;
    uint8_t res=NoFault;

    if(g_AppTxReqflg == 1) //Ӧ�ò㷢�������־Ϊ1
    {
        for(cnt=0 ; cnt < MAX_UDS_TX_BUF_BYTES ; cnt++)
        {
             UdsTxMsgBuf[UDS_Tx_750].buf[cnt] = 0;
        }
        
        if(g_AppTxReq.sa == 0x50)
        {         
            for(cnt=0 ; cnt < g_AppTxReq.buf_len; cnt++)
            {
                UdsTxMsgBuf[UDS_Tx_750].buf[cnt] = g_AppTxReq.buf[cnt];
            }
            
            UdsTxMsgBuf[UDS_Tx_750].buf_len = g_AppTxReq.buf_len;
            res = NWL_udsTxMsg(&UdsTxMsgBuf[UDS_Tx_750]);
        }
       
        g_AppTxReqflg = 0;
    }

    return res;
}

/*****************************************************************************
 �� �� ��  : NW_ErrCodeProc
 ��������  : ����������
 �������  : portBASE_TYPE code  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
#if 1
void NW_ErrCodeProc(uint8_t code)
{
    switch(code)
    {
        case 0x00:
            break;
        default:
            break;
    }
}
#endif
/*****************************************************************************
 �� �� ��  : vCanNetWorkLayerPartTask
 ��������  : CAN���������
 �������  : void *pvParameters  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void vCanNetWorkLayerPartTask(void)
{        
    uint8_t res;   
    if(g_UdsNetRstFlag == 1)
    {
        /* ��ʼ�� UDS buffer ���� ���պͷ���buffer�� Դ��ַ Ŀ���ַ�� */
        g_UdsNetRstFlag = 0;
        udsMsgBufInit(); 
    }
    else
    {           
        res=Nw_CanMsgRxProc();  //����㷢����Ϣ����
        res=Nw_CanMsgTxProc();  //����������Ϣ����
        
        if(res != NoFault)
        {
            NW_ErrCodeProc(res);
        }

    }
    
}

/*****************************************************************************
 �� �� ��  : transDatatoConfirmData
 ��������  : �ӿ�����ת��
 �������  : N_UDDATA_CONFIRM_STRU *ConData  
             N_UDDATA_REQ_STRU *ReqData      
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void transDatatoConfirmData(N_UDDATA_CONFIRM_STRU *ConData,N_UDDATA_REQ_STRU *ReqData)
{    
    ConData->sa=ReqData->sa;
    ConData->ta=ReqData->ta;
    ConData->ae=ReqData->ae;
    ConData->tat=ReqData->tat;
    ConData->pri=ReqData->pri;
}

/*****************************************************************************
 �� �� ��  : NwlTimerCnt
 ��������  : ������ʱ��ģ�飬��Ҫ���ж������ڵ���
 �������  : void  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void NwlTimerCnt(void)
{
    uint8_t cycle; 
    
    if(UdsMsg750Sending  == 1)  //�����Ӧ�������ڷ��� 
    {
        if(CANTFLG_TXE2 == 1)   //��ǰ CAN֡���ͳɹ�
        {
            UdsMsg750TxOk = 1;  
            UdsMsg750Sending = 0;   
        }
        
    }
    
    if(UdsMsg750TxOk == 1)
    {
        /* 709�ۼ��ֽ������ڵ�������֡���ֽ��� �� ��ǰ 709��������Ϣ��Ϊ�� */
        if((UdsTxMsgCnt[UDS_Tx_750].MsgLthCnt >= UdsTxMsgBuf[UDS_Tx_750].buf_len))//&&(uxQueueMessagesWaitingFromISR(Uds709DivQueue)==0))
        {
            ResetDealStat();    //��λ UDS��Ϸ������ڴ���״̬ �Խ����µ���Ϸ�������
            UdsTxMsgCnt[UDS_Tx_750].RxOrTxCmpltFlg = 1;  /* ��750 ���ͻ�����ձ�־Ϊ1 */
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff;  /* �ر�750 StMin��ʱ��  ֹͣ���� */
        }
        NwlTime[UDS_750_STMIN].CntData = 0;   /* 750��ʱ������ */
        UdsMsg750TxOk = 0;    /* 750���ͳɹ���־���� */
    }    

    for ( cycle = 0 ; cycle < NWL_TIM_NUM; cycle++ )
    {
        if(NwlTime[cycle].OnStatus == TimerOn)
        {
            NwlTime[cycle].CntData++;   /* �����ʱ��Ϊ����״̬ ��ʱ�����Լ� */
        }
        else
        {
            NwlTime[cycle].CntData = 0;
        }
    }

    /* 709������״̬Ϊ�������� �� 709 StMin�������ʱ �� (��δ���͵�709��ָ�������ƿ��С
       ����709�����ƿ��СΪ0 ) */
    if((NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750] == ContinueToSend)
        &&(NwlTime[UDS_750_STMIN].CntData >= StminCmpData[UDS_750_STMIN])
        &&((NwlTxBsCnt[UDS_BS_750]<NwlTxBsData[UDS_BS_750])||(NwlTxBsData[UDS_BS_750] == 0)))
    {
        if(TRUE == Nw_UdsSendMsg(UDS_Tx_750)) //����һ֡����֡
        {
        
            NwlTime[UDS_750_STMIN].CntData = 0;    /* ���� StMin��ʱ�� */           
            NwlTxBsCnt[UDS_BS_750]++;   //���� Bs BlockSize ������ 1
        }
    }
    else
    {
        /* 750���͵�֡�Ѵ���С�� ���� �����750��������״̬Ϊ�ȴ�(��������֡��) */
        if((NwlTxBsCnt[UDS_Tx_750] >= NwlTxBsData[UDS_Tx_750])  \
            ||(NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750] == Wait)) 
        {
            if(NwlTime[UDS_750_NBs].CntData > N_BS_MAX_TIME)
            {
                //����������֡�ȴ���ʱ
                NwlTime[UDS_750_NBs].OnStatus = TimerOff;
                NwlTime[UDS_750_NBs].CntData = 0;
               
                initTxMsgData(UDS_Tx_750);
                ResetDealStat();    //��λ UDS��Ϸ������ڴ���״̬ �Խ����µ���Ϸ�������
                /* ��Ӧ�ò㱨�� */
            }
            if(NwlTxBsData[UDS_Tx_750] != 0)
            {
                if(NwlTime[UDS_750_STMIN].OnStatus == TimerOn)
                {
                    NwlTime[UDS_750_STMIN].OnStatus = TimerOff;
                    NwlTime[UDS_750_STMIN].CntData = 0;    /* ���� StMin��ʱ�� */ 
                }
            }
        }
        else
        {
            if(NwlTime[UDS_750_NCs].CntData > N_CS_MAX)//  N_CR_MAX_TIME N_CS��Ӧ�ò����
            {
              
            }
        }
    }
    
    if(NwlTime[UDS_740_NCr].OnStatus == TimerOn)
    {
        if(NwlTime[UDS_740_NCr].CntData > N_CR_MAX_TIME)    //�Ǳ���� 740 ��������֡��ʱ
        {
            initRxMsgData(UDS_RX_740 , N_TIMEOUT_Cr);
            //initTxMsgData(UDS_Tx_750);
            NwlTime[UDS_740_NCr].OnStatus = TimerOff;
        }
    }

}

    
