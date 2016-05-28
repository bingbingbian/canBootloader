#include "i15765MsgProc.h"
#include "UdsApp.h"
#include "iso_15765_2_config.h" 
#include "CanNetWorkLayer.h"
#include "port.h"
#include "DrvMSCAN.h"
#include "TxFuncs.h"

_FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM]; //�����������״̬

uint8_t NwlTxBsData[2];     //����ʱ �ӿͻ������ص� BS ���С ����
uint8_t NwlTxBsCnt[2];      //����㷢�� Bs���� 
uint16_t StminCmpData[2];   //����ʱ �ӿͻ������ص� StMin ��С��� ����

uint8_t UdsMsg750TxOk = 0;  //0x750 ��Ӧ���ķ��ͳɹ���־

uint8_t g_RxChannel;       //��ǰ���� bufferͨ�� 0x740 ���� 0x7DF
SEG_SEND_ENUM SegSendFlag; // ����֡��ʽ���ͱ�־
uint8_t UdsMsg750Sending;  // uds 750�������ڷ��ͱ�־
uint8_t g_CFSendFlag=0; //

N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];  //��ϱ��Ľ��� buffer
N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];  //��ϱ��ķ��� buffer
_UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM];    //��ϱ��Ľ���״̬������
_UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM];    //��ϱ��ķ���״̬������

#pragma CODE_SEG NON_BANKED

/*****************************************************************************
 �� �� ��  : udsclrMsgBufData
 ��������  : ���UDS��Ϣbuffer
 �������  : uint8_t *Point     
             uint8_t MsgLength  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void udsclrMsgBufData(uint8_t *Point,uint8_t MsgLength)
{
    uint16_t cycle;
    
    for ( cycle = 0 ; cycle < MsgLength; cycle++ )
    {
        Point[cycle]=0;
    }
}

/*****************************************************************************
 �� �� ��  : initTxMsgData
 ��������  : ��ʼ��������Ϣ
 �������  : uint8_t MsgChannel  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void initTxMsgData(uint8_t MsgChannel)
{    
    UdsTxMsgBuf[MsgChannel].buf_len=0;
    udsclrMsgBufData(UdsTxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    UdsTxMsgCnt[MsgChannel].MsgLthCnt=0;
    UdsTxMsgCnt[MsgChannel].SnCnt=0;
    UdsTxMsgCnt[MsgChannel].SnAddData=0;
    UdsTxMsgCnt[MsgChannel].RxOrTxCmpltFlg=0;
    
}

/*****************************************************************************
 �� �� ��  : initRxMsgData
 ��������  : ��ʼ��������Ϣ
 �������  : uint8_t MsgChannel  
             uint8_t result      
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��25��������
    ��    ��   : fsfs
    �޸�����   : �����ɺ���

*****************************************************************************/
void initRxMsgData(uint8_t MsgChannel,uint8_t result)
{    
    UdsRxMsgBuf[MsgChannel].buf_len=0;
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    UdsRxMsgCnt[MsgChannel].MsgLthCnt=0;
    UdsRxMsgCnt[MsgChannel].SnCnt=0;    
    UdsRxMsgCnt[MsgChannel].SnAddData=0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg=0;
    UdsRxMsgBuf[MsgChannel].result = result;
}

/*****************************************************************************
 �� �� ��  : udsMsgBufInit
 ��������  : ��ʼ�������Ϣ
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
void udsMsgBufInit(void)
{
    uint16_t cycle,tmp;
       
    UdsRxMsgBuf[UDS_RX_740].sa=0x40;    //���ؽ��յ�ַ
    UdsRxMsgBuf[UDS_RX_740].ta=0x50;    //���͵�ַ ( ��Ӧ CAN_ID )
    UdsRxMsgBuf[UDS_RX_740].tat=0;  
    
    UdsRxMsgBuf[UDS_RX_7DF].sa=0xdf;    //���ؽ��յ�ַ
    UdsRxMsgBuf[UDS_RX_7DF].ta=0x50;    //���͵�ַ ( ��Ӧ CAN_ID )
    UdsRxMsgBuf[UDS_RX_7DF].tat=1;  
    
    UdsTxMsgBuf[UDS_Tx_750].sa=0x50;    //���͵�ַ ( �Ǳ���Ӧ CAN_ID )
    UdsTxMsgBuf[UDS_Tx_750].ta=0xff;
    UdsTxMsgBuf[UDS_Tx_750].tat=2;  
    
    tmp=UDS_RX_MSG_NUM;
    for ( cycle = 0 ; cycle < tmp ; cycle++ )
    {
        if(cycle < UDS_RX_MSG_NUM)
        {
            UdsRxMsgCnt[cycle].SnCnt=0;            
            UdsRxMsgCnt[cycle].SnAddData=0;
            UdsRxMsgCnt[cycle].MsgLthCnt=0;            
            UdsRxMsgCnt[cycle].RxOrTxCmpltFlg=1;
            
            UdsRxMsgBuf[cycle].ae=0;
            UdsRxMsgBuf[cycle].pri=0;        
            UdsRxMsgBuf[cycle].buf_len=0;
            udsclrMsgBufData(UdsRxMsgBuf[cycle].buf, MAX_UDS_TX_BUF_BYTES);
            UdsRxMsgBuf[cycle].result=N_OK;
        }
        
        if(cycle < UDS_TX_MSG_NUM)
        {            
            UdsTxMsgCnt[cycle].SnCnt=0;        
            UdsTxMsgCnt[cycle].SnAddData=0;
            UdsTxMsgCnt[cycle].MsgLthCnt=0;            
            UdsTxMsgCnt[cycle].RxOrTxCmpltFlg=1;
            
            UdsTxMsgBuf[cycle].ae=0;
            UdsTxMsgBuf[cycle].pri=0;        
            UdsTxMsgBuf[cycle].buf_len=0;
            udsclrMsgBufData(UdsTxMsgBuf[cycle].buf, MAX_UDS_TX_BUF_BYTES);            
        }
    }    

}

/*****************************************************************************
 �� �� ��  : NWL_CpyData
 ��������  : ���������Ϣ
 �������  : uint8_t *Dr        
             uint8_t *Sr        
             uint8_t MsgLength  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
void NWL_CpyData(uint8_t *Dr,uint8_t *Sr,uint8_t MsgLength)
{
    uint8_t cycle;
    for ( cycle = 0 ; cycle < MsgLength ; cycle++ )
    {
        Dr[cycle] = Sr[cycle];
    }
}

/*****************************************************************************
 �� �� ��  : NWL_SFMsgProc
 ��������  : ��֡��ϱ��Ĵ���
 �������  : CAN_BUF_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_SFMsgProc(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t res=TRUE;
    uint8_t MsgChannel=0,tmp=0;
    switch ( pMsg->Id )
    {
        case 0x740 :
            MsgChannel = UDS_RX_740;    //ѡ�� 740����ͨ��
            break;
        case 0x7df :
            MsgChannel = UDS_RX_7DF;    //ѡ�� 7DF����ͨ��          
            break;
        default:
            return FALSE;
    }

    if(CheckDealStat() == 1)    //����Ϸ������ڴ��� ���Ե�ǰ�µ�����
    {
        return FALSE;
    }
    
    tmp = pMsg->Data[0]&0x0f;   //��ȡ SF_DLC
    if((tmp >= 8)||(tmp == 0)) //��֡��Ч���ݳ��Ȳ��ܴ��� 7 �Ҳ���Ϊ��
    {
        return FALSE;
    }
    else if(pMsg->Length < (tmp+1)) // CAN_DLC ��ӦС�� SF_DLC+1 ( sizeof(N_PCI) + SF_DLC)
    {
        return FALSE;    
    }
    
    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    //���ý���buffer��Ч���ݳ���
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;        //����buffer֡�������
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;    //����buffer����֡��������
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 0;    //����buffer����֡�ѽ����ܳ�������
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 1;   //���ջ����ͳɹ���־�� 1
    
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->Data[1]),tmp); //������֡��Ч���ݵ����� buffer
    UdsRxMsgBuf[MsgChannel].result = N_OK;
    
    //��Ӧ�ò��ṩ���ճɹ�ָʾ
    g_RxChannel = MsgChannel;
    g_UdsRxOkFlag = 1;    //��λ��Ϣ������ճɹ���־
    //start RxQueue Timer
    
    return res;
}

/*****************************************************************************
 �� �� ��  : NWL_FFMsgProc
 ��������  : ���ն�֡������֡����
 �������  : CAN_BUF_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_FFMsgProc(UDS_CAN_RX_STRU *pMsg) // CAN_RX_STRU *pMsg)
{
    uint8_t MsgChannel=0;
    uint16_t tmp=0;
    
    switch ( pMsg->Id )
    {
        case 0x740 :
            {
                MsgChannel=UDS_RX_740;               
            }
            break;
        case 0x7df :
            {
                //MsgChannel=UDS_RX_7DF; 
                return FALSE;  //����Ѱַ��֧�ֶ�֡ ��������֡
            }
            break;
        default:
            return FALSE;
    }
    
    if(CheckDealStat() == 1)    //����Ϸ������ڴ��� ���Ե�ǰ�µ�����
    {
        return FALSE;
    }
    
    tmp = (uint16_t)((pMsg->Data[0]&0x0f)<<8)+(pMsg->Data[1]); //FF֡ ��һ֡ ���ݳ��������
    if(tmp<8)  //����֡�ĵ�һ�볤����Ӧ�ô��ڵ��� 8
    {
        return FALSE;
    }
    else if(tmp>MAX_UDS_TX_BUF_BYTES)  // ����֡���ȴ������Ľ��� buffer
    {        
        initRxMsgData(MsgChannel , N_WRONG_SN);
        
        NwlFsStatus[MsgChannel] = OverFlow;        
        NWL_TxFlowControlFrameProc(pMsg);   //����������״̬Ϊ OverFlow��������֡ 
        
        return FALSE;
    }

    //�������ݷ��ͽ��ո�������
    NwlFsStatus[MsgChannel] = ContinueToSend;        
    NWL_TxFlowControlFrameProc(pMsg); //����������֡
        
    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 0;
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 6;  //��֡���� 6���ֽڵ���Ч����
    //���� uds���� buffer
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES); 
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->Data[2]),6); //������֡�е���Ч����

    //������������֡�ȴ�ʱ�䶨ʱ��
    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].OnStatus = TimerOn;
        NwlTime[UDS_740_NCr].CntData = 0; //���յ����� ���� NCr
    }

    if(NwlTime[UDS_750_STMIN].OnStatus == TimerOn)
    {
        NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
        NwlTime[UDS_750_STMIN].CntData = 0;
    }
    return TRUE;
}

/*****************************************************************************
 �� �� ��  : NWL_CFMsgProc
 ��������  : ��������֡���Ĵ���
 �������  : CAN_BUF_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_CFMsgProc(UDS_CAN_RX_STRU *pMsg)
{ 
    uint8_t MsgChannel=0;
    uint16_t tmp=0;
    uint8_t *Point;
    
    switch ( pMsg->Id )   //������� ID
    {
        case 0x740:
            MsgChannel = UDS_RX_740;
            break;
        case 0x7df:
            MsgChannel = UDS_RX_7DF;              
            break;
        default:
            return FALSE;
    }

    if(pMsg->Length < 8)    //����֡ CAN_DLCС�� 8 ������
    {
        return FALSE;
    }
    
    if(UdsRxMsgBuf[MsgChannel].buf_len == 0)    //��������� bufferΪ�� ����δ���յ���Ч��֡ ����
    {
        return FALSE;
    }
    
    tmp=pMsg->Data[0]&0x0f;  //��ȡ������֡��֡���  
    UdsRxMsgCnt[MsgChannel].SnAddData++;
    
    //���յ���֡������ �����ͷ��������͵�֡��
    if((UdsRxMsgCnt[MsgChannel].SnAddData%N_INIT_BS) == 0) 
    {        
        NWL_TxFlowControlFrameProc(pMsg);   //����������֡
    }
    
    UdsRxMsgCnt[MsgChannel].SnCnt=UdsRxMsgCnt[MsgChannel].SnAddData%0x0f;
    if(tmp != (UdsRxMsgCnt[MsgChannel].SnCnt))    //������յ���֡��Ų���
    {        
        initRxMsgData(MsgChannel , N_WRONG_SN);

        if(MsgChannel == UDS_RX_740)
        {
            NwlTime[UDS_740_NCr].OnStatus = TimerOff;
            NwlTime[UDS_740_NCr].CntData = 0;
        }
        return FALSE;
    }

    tmp = UdsRxMsgBuf[MsgChannel].buf_len-UdsRxMsgCnt[MsgChannel].MsgLthCnt;
    
    if(tmp > 7 )
    {
        tmp=7;        
    }
    
    Point=UdsRxMsgBuf[MsgChannel].buf+(UdsRxMsgCnt[MsgChannel].MsgLthCnt);
    NWL_CpyData(Point,&(pMsg->Data[1]),(uint8_t)tmp); //���ƺ���֡���ݵ� uds���� Buffer
    
    UdsRxMsgCnt[MsgChannel].MsgLthCnt += tmp; //����֡��Ч�����ֽ����Լ�

    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].CntData = 0;   //���յ����� ���� NCr
    }
    
    //���յ���֡��ָʾ������֡�ֽ�����С���������� ��Ӧ�ò��ύָʾ
    if(UdsRxMsgCnt[MsgChannel].MsgLthCnt == UdsRxMsgBuf[MsgChannel].buf_len)
    {        
        UdsRxMsgBuf[MsgChannel].result = N_OK;
        //��Ӧ�ò��ṩ���ճɹ�ָʾ
        g_RxChannel = MsgChannel;
        g_UdsRxOkFlag = 1;    //��λ���Ľ��ճɹ���־
    }
    
    return TRUE;
}

/*****************************************************************************
 �� �� ��  : NWL_FCFMsgProc
 ��������  : ��Ͻ���������֡���Ĵ���
 �������  : CAN_BUF_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_FCFMsgProc(UDS_CAN_RX_STRU *pMsg)
{
    (void)NWL_RxFlowControlFrameProc(pMsg);   
    return TRUE;            
}


uint8_t NWL_udsRxMsg(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t res=TRUE;
    switch(pMsg->Data[0]&0xf0)
    {
        case 0x00 : //��֡
            res = NWL_SFMsgProc(pMsg);
            break;
        case 0x10 : //��֡
            res = NWL_FFMsgProc(pMsg);
            break;
        case 0x20 : //����֡
            res = NWL_CFMsgProc(pMsg);
            break;
        case 0x30 : //������֡
            res = NWL_FCFMsgProc(pMsg); 
            break;
        default:
            res=FALSE;
    }
	return res;
}

/*****************************************************************************
 �� �� ��  : Nw_UdsWriteTxMsgToQueue
 ��������  : д��֡�������������д���
 �������  : xQueueHandle *Queue  
             uint8_t Channel      
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t Nw_UdsSendMsg(uint8_t Channel)
{
    CAN_TX_STRU  TxMsg;
    uint8_t tmp,cycle;
    uint8_t *Point;
    
    TxMsg.Id = UdsTxMsgBuf[Channel].sa+DIAG_BASE_ADRESS; /* 0x50+0x700 */
    TxMsg.Length = 8;

    if(SegSendFlag == FirstFrame)   //�����͵�����֡
    {
        TxMsg.Data[0] = 0x10+((UdsTxMsgBuf[Channel].buf_len>>8)&0x0f);
        TxMsg.Data[1] = UdsTxMsgBuf[Channel].buf_len&0xff;
        Point=UdsTxMsgBuf[Channel].buf;
        NWL_CpyData(&(TxMsg.Data[2]), Point, 6);
        UdsTxMsgCnt[Channel].MsgLthCnt=6;
        UdsTxMsgCnt[Channel].RxOrTxCmpltFlg = 0;
        UdsTxMsgCnt[Channel].SnCnt=0;

        //������֡
        Send_Can_Message(TxMsg);
        UdsMsg750Sending = 1;               
        NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750]=Wait; //�ȴ�����������֡
        
        NwlTime[UDS_750_NBs].OnStatus = TimerOn;    //�����ȴ�����������֡��ʱ�� N_Bs 
        NwlTime[UDS_750_NBs].CntData = 0;
        SegSendFlag = ConsecutiveFrame;     //����������֡Ϊ����֡
        return TRUE;
    }
    else if(SegSendFlag == ConsecutiveFrame)    //������֡Ϊ����֡
    {
        //��ǰӦ�ò������ͱ��Ļ�δ������
        if(UdsTxMsgBuf[Channel].buf_len > UdsTxMsgCnt[Channel].MsgLthCnt)
        {
            UdsTxMsgCnt[Channel].SnCnt++; //ÿ���Լ�  ȷ�ϵ�һ������֡�� ���Ϊ 1
            if(UdsTxMsgCnt[Channel].SnCnt>0x0f) //����Ŵ��� 15ʱĬ������ 0
            {
                UdsTxMsgCnt[Channel].SnCnt=0;
            }

            for ( cycle = 0 ; cycle < 8 ; cycle++ ) //�������㷢��֡���� buffer
            {
                TxMsg.Data[cycle]=0;
            }
            //��������֡
            TxMsg.Data[0]=0x20+(UdsTxMsgCnt[Channel].SnCnt&0x0f);
            tmp=UdsTxMsgBuf[Channel].buf_len-UdsTxMsgCnt[Channel].MsgLthCnt;
            
            if ( tmp > 7 )
            {
                tmp=7;  //����֡�����Ч���ݳ���Ϊ 7
            }

            Point=UdsTxMsgBuf[Channel].buf+UdsTxMsgCnt[Channel].MsgLthCnt;
            NWL_CpyData(&(TxMsg.Data[1]), Point, tmp);
            UdsTxMsgCnt[Channel].MsgLthCnt += tmp;

            Send_Can_Message(TxMsg);
            UdsMsg750Sending = 1;
            return TRUE;
        }
    }
    
    return FALSE;
}

/*****************************************************************************
 �� �� ��  : NWL_udsTxMsg
 ��������  : ��ϱ��ķ��ʹ���
 �������  : N_UDDATA_REQ_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_udsTxMsg(N_UDDATA_REQ_STRU *pMsg)
{
    CAN_TX_STRU TxTmp;
    uint8_t cycle;
    uint8_t TxNumTmp;

    for ( cycle = 0 ; cycle < 8 ; cycle++ )
    {
        TxTmp.Data[cycle]=0;
    }

    if(pMsg->buf_len<8) //�����͵����ݳ���С�� 8 ��֡���ɷ���
    {
        TxTmp.Id=pMsg->sa+DIAG_BASE_ADRESS;
        TxTmp.Length=8;
        TxTmp.Data[0]=(uint8_t)pMsg->buf_len;
        NWL_CpyData(&(TxTmp.Data[1]),pMsg->buf,(uint8_t)pMsg->buf_len); 

        switch ( pMsg->sa )
        {
            case 0x50 :
                //��֡����
                Send_Can_Message(TxTmp);
                UdsTxMsgCnt[UDS_Tx_750].MsgLthCnt = pMsg->buf_len;
                UdsMsg750Sending = 1;
                break;
            default:
                initTxMsgData(UDS_Tx_750);
                // ��Ӧ�ò㱨��
                return FALSE;
        }
        
    }
    else //���ݳ��ȴ��� ���� 8ʱ ��ֶη���
    {
        switch ( pMsg->sa )
        {
            case 0x50 :
                TxNumTmp=UDS_Tx_750;
                break;
            default:
                initTxMsgData(UDS_Tx_750);

                // ��Ӧ�ò�ȷ��
                return FALSE;
        }
              
        UdsTxMsgCnt[TxNumTmp].MsgLthCnt=0;
        UdsTxMsgCnt[TxNumTmp].SnCnt=0;
        UdsTxMsgCnt[TxNumTmp].RxOrTxCmpltFlg=0;
        SegSendFlag = FirstFrame; //��λ����֡��ʽ���ͱ�־

        Nw_UdsSendMsg(UDS_Tx_750);        
        
    }

    return TRUE;
}

/*****************************************************************************
 �� �� ��  : NWL_RxFlowControlFrameProc
 ��������  : ����������֡����
 �������  : CAN_BUF_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2013��1��28��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_RxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t Txtmp,Bstmp,Stmtmp;
    _FS_STATUS_ENUM FC_Stat;

    if(pMsg->Id == 0x740)
    {    
        Txtmp=UDS_Tx_750;
        Bstmp=UDS_BS_750;
        Stmtmp=UDS_750_STMIN;
    }
    else
    {
        return FALSE;
    }
    if(pMsg->Length < 3)
    {
        return FALSE;   //����Ƿ��͵�������֡�� DLCС�� 3 �Ǳ��ظ�����֡   
    }
    
    FC_Stat = (_FS_STATUS_ENUM)(pMsg->Data[0]&0x0f);
    //if(NwlTime[UDS_750_STMIN].OnStatus == TimerOff)
    {
        /* ��ȡ������״̬ 0-ContinueToSend  1-Wait  2-OverFlow  */
        if(g_CFSendFlag ==  0)
        {
            NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] = FC_Stat;
        }
        
        NwlTxBsData[Bstmp] = pMsg->Data[1];     //��ȡ������֡֡�е�  BS����
        StminCmpData[Stmtmp] = pMsg->Data[2]+1;   //��ȡ������֡�е� StMin ���� 
                                                  //( �����������ǵ� STmin ƫС �� �� 1)
        NwlTxBsCnt[Bstmp] = 0;                  //����㷢�� Bs��������                                          
        
        if(NwlTime[UDS_750_NBs].OnStatus == TimerOn)
        {
            NwlTime[UDS_750_NBs].OnStatus = TimerOff;    //�ر�������֡�ȴ���ʱ�� N_Bs 
            NwlTime[UDS_750_NBs].CntData = 0;            //���� N_Bs ��ʱ��

        }  
        
        if(StminCmpData[Stmtmp] > N_CS_MAX) //������շ����͵�������֡�������� N_CS_MAX 
        {
            StminCmpData[Stmtmp]  = N_CS_MAX;
        }
        else if(StminCmpData[Stmtmp] < N_INIT_STMIN)
        {
            StminCmpData[Stmtmp] = N_INIT_STMIN;
        }
        if(NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] == ContinueToSend)
        {
            if(UdsTxMsgBuf[UDS_Tx_750].buf_len !=0 )
            {
                //���յ������͵�������֡���� ST_MIN��ʱ��
                g_CFSendFlag =  1;
                NwlTime[UDS_750_STMIN].OnStatus = TimerOn; 
                NwlTime[UDS_750_STMIN].CntData = 0;
            }
        }
        else if(NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] == Wait)
        {
            ResetDealStat();   //��λ UDS��Ϸ������ڴ���״̬ �Խ����µ���Ϸ�������
            
            //NwlTime[UDS_740_NCr].OnStatus = TimerOn;
            //NwlTime[UDS_740_NCr].CntData = 0;
            initTxMsgData(UDS_Tx_750);
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
        else if(NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] == OverFlow) /* ��������֡����ʹ�Ǳ��ж϶�֡���ĵķ��� */
        {
            ResetDealStat();    //��λ UDS��Ϸ������ڴ���״̬ �Խ����µ���Ϸ�������
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
        else //������ദ��
        {
            ResetDealStat();    //��λ UDS��Ϸ������ڴ���״̬ �Խ����µ���Ϸ�������
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff;
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
    }

    return TRUE;
}

/*****************************************************************************
 �� �� ��  : NWL_TxFlowControlFrameProc
 ��������  : UDS����㷢��������֡������
 �������  : UDS_CAN_RX_STRU *pMsg  
 �������  : ��
 �� �� ֵ  : 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2014��7��23��������
    ��    ��   : xiaobo
    �޸�����   : �����ɺ���

*****************************************************************************/
uint8_t NWL_TxFlowControlFrameProc(UDS_CAN_RX_STRU *pMsg)
{
    CAN_TX_STRU TxMsg;
    uint8_t cycle;

    for ( cycle = 0 ; cycle < 8 ; cycle++ )
    {
        TxMsg.Data[cycle]=0;
    }

    switch ( pMsg->Id )
    {
        case 0x740 :
            TxMsg.Id = 0x750;
            TxMsg.Data[0] = (uint8_t)NwlFsStatus[UDS_RX_740];
            break;
        case 0x7df :
            TxMsg.Id = 0x750;
            TxMsg.Data[0] = (uint8_t)NwlFsStatus[UDS_RX_7DF];            
            break;
        default:            
            return FALSE;
    }

    TxMsg.Data[0] |= (0X3 << 4);    //������֡
    TxMsg.Data[1] |= N_INIT_BS;     //�����Ǳ�������յ� BS
    TxMsg.Data[2] |= N_INIT_STMIN;  //�����Ǳ�������յ� STmin
    TxMsg.Length = 8;

    Send_Can_Message(TxMsg);  //���� CAN�����ӿں��� ���������� CAN֡

    return TRUE;
}

#pragma CODE_SEG DEFAULT




