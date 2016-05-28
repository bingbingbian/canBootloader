#include "i15765MsgProc.h"
#include "UdsApp.h"
#include "iso_15765_2_config.h" 
#include "CanNetWorkLayer.h"
#include "port.h"
#include "DrvMSCAN.h"
#include "TxFuncs.h"

_FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM+UDS_TX_MSG_NUM]; //网络层流控制状态

uint8_t NwlTxBsData[2];     //发送时 从客户机返回的 BS 块大小 参数
uint8_t NwlTxBsCnt[2];      //网络层发送 Bs计数 
uint16_t StminCmpData[2];   //发送时 从客户机返回的 StMin 最小间隔 参数

uint8_t UdsMsg750TxOk = 0;  //0x750 响应报文发送成功标志

uint8_t g_RxChannel;       //当前接收 buffer通道 0x740 或者 0x7DF
SEG_SEND_ENUM SegSendFlag; // 连续帧形式发送标志
uint8_t UdsMsg750Sending;  // uds 750报文正在发送标志
uint8_t g_CFSendFlag=0; //

N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];  //诊断报文接收 buffer
N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];  //诊断报文发送 buffer
_UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM];    //诊断报文接收状态控制用
_UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM];    //诊断报文发送状态控制用

#pragma CODE_SEG NON_BANKED

/*****************************************************************************
 函 数 名  : udsclrMsgBufData
 功能描述  : 清除UDS消息buffer
 输入参数  : uint8_t *Point     
             uint8_t MsgLength  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
 函 数 名  : initTxMsgData
 功能描述  : 初始化发送消息
 输入参数  : uint8_t MsgChannel  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
 函 数 名  : initRxMsgData
 功能描述  : 初始化接收消息
 输入参数  : uint8_t MsgChannel  
             uint8_t result      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月25日星期五
    作    者   : fsfs
    修改内容   : 新生成函数

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
 函 数 名  : udsMsgBufInit
 功能描述  : 初始化诊断消息
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

*****************************************************************************/
void udsMsgBufInit(void)
{
    uint16_t cycle,tmp;
       
    UdsRxMsgBuf[UDS_RX_740].sa=0x40;    //本地接收地址
    UdsRxMsgBuf[UDS_RX_740].ta=0x50;    //发送地址 ( 响应 CAN_ID )
    UdsRxMsgBuf[UDS_RX_740].tat=0;  
    
    UdsRxMsgBuf[UDS_RX_7DF].sa=0xdf;    //本地接收地址
    UdsRxMsgBuf[UDS_RX_7DF].ta=0x50;    //发送地址 ( 响应 CAN_ID )
    UdsRxMsgBuf[UDS_RX_7DF].tat=1;  
    
    UdsTxMsgBuf[UDS_Tx_750].sa=0x50;    //发送地址 ( 仪表响应 CAN_ID )
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
 函 数 名  : NWL_CpyData
 功能描述  : 拷贝诊断消息
 输入参数  : uint8_t *Dr        
             uint8_t *Sr        
             uint8_t MsgLength  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
 函 数 名  : NWL_SFMsgProc
 功能描述  : 单帧诊断报文处理
 输入参数  : CAN_BUF_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t NWL_SFMsgProc(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t res=TRUE;
    uint8_t MsgChannel=0,tmp=0;
    switch ( pMsg->Id )
    {
        case 0x740 :
            MsgChannel = UDS_RX_740;    //选择 740接收通道
            break;
        case 0x7df :
            MsgChannel = UDS_RX_7DF;    //选择 7DF接收通道          
            break;
        default:
            return FALSE;
    }

    if(CheckDealStat() == 1)    //有诊断服务正在处理 忽略当前新的请求
    {
        return FALSE;
    }
    
    tmp = pMsg->Data[0]&0x0f;   //提取 SF_DLC
    if((tmp >= 8)||(tmp == 0)) //单帧有效数据长度不能大于 7 且不能为零
    {
        return FALSE;
    }
    else if(pMsg->Length < (tmp+1)) // CAN_DLC 不应小于 SF_DLC+1 ( sizeof(N_PCI) + SF_DLC)
    {
        return FALSE;    
    }
    
    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    //设置接收buffer有效数据长度
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;        //接收buffer帧序号清零
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;    //接收buffer连续帧计数清零
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 0;    //接收buffer连续帧已接收总长度清零
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 1;   //接收或则发送成功标志置 1
    
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->Data[1]),tmp); //拷贝单帧有效数据到接收 buffer
    UdsRxMsgBuf[MsgChannel].result = N_OK;
    
    //向应用层提供接收成功指示
    g_RxChannel = MsgChannel;
    g_UdsRxOkFlag = 1;    //置位消息请求接收成功标志
    //start RxQueue Timer
    
    return res;
}

/*****************************************************************************
 函 数 名  : NWL_FFMsgProc
 功能描述  : 接收多帧报文首帧处理
 输入参数  : CAN_BUF_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
                return FALSE;  //功能寻址不支持多帧 不处理首帧
            }
            break;
        default:
            return FALSE;
    }
    
    if(CheckDealStat() == 1)    //有诊断服务正在处理 忽略当前新的请求
    {
        return FALSE;
    }
    
    tmp = (uint16_t)((pMsg->Data[0]&0x0f)<<8)+(pMsg->Data[1]); //FF帧 第一帧 数据长度域解析
    if(tmp<8)  //连续帧的第一针长度域应该大于等于 8
    {
        return FALSE;
    }
    else if(tmp>MAX_UDS_TX_BUF_BYTES)  // 连续帧长度大于最大的接收 buffer
    {        
        initRxMsgData(MsgChannel , N_WRONG_SN);
        
        NwlFsStatus[MsgChannel] = OverFlow;        
        NWL_TxFlowControlFrameProc(pMsg);   //发送流控制状态为 OverFlow的流控制帧 
        
        return FALSE;
    }

    //连续数据发送接收更新正常
    NwlFsStatus[MsgChannel] = ContinueToSend;        
    NWL_TxFlowControlFrameProc(pMsg); //发送流控制帧
        
    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 0;
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 6;  //首帧中有 6个字节的有效数据
    //清零 uds接收 buffer
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES); 
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->Data[2]),6); //拷贝首帧中的有效数据

    //开启接收连续帧等待时间定时器
    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].OnStatus = TimerOn;
        NwlTime[UDS_740_NCr].CntData = 0; //接收到数据 清零 NCr
    }

    if(NwlTime[UDS_750_STMIN].OnStatus == TimerOn)
    {
        NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
        NwlTime[UDS_750_STMIN].CntData = 0;
    }
    return TRUE;
}

/*****************************************************************************
 函 数 名  : NWL_CFMsgProc
 功能描述  : 接收连续帧报文处理
 输入参数  : CAN_BUF_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t NWL_CFMsgProc(UDS_CAN_RX_STRU *pMsg)
{ 
    uint8_t MsgChannel=0;
    uint16_t tmp=0;
    uint8_t *Point;
    
    switch ( pMsg->Id )   //解析诊断 ID
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

    if(pMsg->Length < 8)    //连续帧 CAN_DLC小于 8 不接收
    {
        return FALSE;
    }
    
    if(UdsRxMsgBuf[MsgChannel].buf_len == 0)    //如果待接收 buffer为空 即还未接收到有效首帧 返回
    {
        return FALSE;
    }
    
    tmp=pMsg->Data[0]&0x0f;  //获取流控制帧的帧序号  
    UdsRxMsgCnt[MsgChannel].SnAddData++;
    
    //接收到的帧数等于 允许发送方连续发送的帧数
    if((UdsRxMsgCnt[MsgChannel].SnAddData%N_INIT_BS) == 0) 
    {        
        NWL_TxFlowControlFrameProc(pMsg);   //发送流控制帧
    }
    
    UdsRxMsgCnt[MsgChannel].SnCnt=UdsRxMsgCnt[MsgChannel].SnAddData%0x0f;
    if(tmp != (UdsRxMsgCnt[MsgChannel].SnCnt))    //如果接收到的帧序号不对
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
    NWL_CpyData(Point,&(pMsg->Data[1]),(uint8_t)tmp); //复制后续帧数据到 uds接收 Buffer
    
    UdsRxMsgCnt[MsgChannel].MsgLthCnt += tmp; //连续帧有效数据字节数自加

    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].CntData = 0;   //接收到数据 清零 NCr
    }
    
    //接收到首帧中指示的连续帧字节数大小的完整数据 向应用层提交指示
    if(UdsRxMsgCnt[MsgChannel].MsgLthCnt == UdsRxMsgBuf[MsgChannel].buf_len)
    {        
        UdsRxMsgBuf[MsgChannel].result = N_OK;
        //向应用层提供接收成功指示
        g_RxChannel = MsgChannel;
        g_UdsRxOkFlag = 1;    //置位报文接收成功标志
    }
    
    return TRUE;
}

/*****************************************************************************
 函 数 名  : NWL_FCFMsgProc
 功能描述  : 诊断接收流控制帧报文处理
 输入参数  : CAN_BUF_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
        case 0x00 : //单帧
            res = NWL_SFMsgProc(pMsg);
            break;
        case 0x10 : //首帧
            res = NWL_FFMsgProc(pMsg);
            break;
        case 0x20 : //连续帧
            res = NWL_CFMsgProc(pMsg);
            break;
        case 0x30 : //流控制帧
            res = NWL_FCFMsgProc(pMsg); 
            break;
        default:
            res=FALSE;
    }
	return res;
}

/*****************************************************************************
 函 数 名  : Nw_UdsWriteTxMsgToQueue
 功能描述  : 写多帧报文数据至队列处理
 输入参数  : xQueueHandle *Queue  
             uint8_t Channel      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t Nw_UdsSendMsg(uint8_t Channel)
{
    CAN_TX_STRU  TxMsg;
    uint8_t tmp,cycle;
    uint8_t *Point;
    
    TxMsg.Id = UdsTxMsgBuf[Channel].sa+DIAG_BASE_ADRESS; /* 0x50+0x700 */
    TxMsg.Length = 8;

    if(SegSendFlag == FirstFrame)   //待发送的是首帧
    {
        TxMsg.Data[0] = 0x10+((UdsTxMsgBuf[Channel].buf_len>>8)&0x0f);
        TxMsg.Data[1] = UdsTxMsgBuf[Channel].buf_len&0xff;
        Point=UdsTxMsgBuf[Channel].buf;
        NWL_CpyData(&(TxMsg.Data[2]), Point, 6);
        UdsTxMsgCnt[Channel].MsgLthCnt=6;
        UdsTxMsgCnt[Channel].RxOrTxCmpltFlg = 0;
        UdsTxMsgCnt[Channel].SnCnt=0;

        //发送首帧
        Send_Can_Message(TxMsg);
        UdsMsg750Sending = 1;               
        NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750]=Wait; //等待接收流控制帧
        
        NwlTime[UDS_750_NBs].OnStatus = TimerOn;    //开启等待接收流控制帧定时器 N_Bs 
        NwlTime[UDS_750_NBs].CntData = 0;
        SegSendFlag = ConsecutiveFrame;     //后续待发送帧为连续帧
        return TRUE;
    }
    else if(SegSendFlag == ConsecutiveFrame)    //待发送帧为连续帧
    {
        //当前应用层请求发送报文还未发送完
        if(UdsTxMsgBuf[Channel].buf_len > UdsTxMsgCnt[Channel].MsgLthCnt)
        {
            UdsTxMsgCnt[Channel].SnCnt++; //每次自加  确认第一个连续帧的 序号为 1
            if(UdsTxMsgCnt[Channel].SnCnt>0x0f) //当序号大于 15时默认跳回 0
            {
                UdsTxMsgCnt[Channel].SnCnt=0;
            }

            for ( cycle = 0 ; cycle < 8 ; cycle++ ) //先清理零发送帧数据 buffer
            {
                TxMsg.Data[cycle]=0;
            }
            //发送连续帧
            TxMsg.Data[0]=0x20+(UdsTxMsgCnt[Channel].SnCnt&0x0f);
            tmp=UdsTxMsgBuf[Channel].buf_len-UdsTxMsgCnt[Channel].MsgLthCnt;
            
            if ( tmp > 7 )
            {
                tmp=7;  //连续帧最大有效数据长度为 7
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
 函 数 名  : NWL_udsTxMsg
 功能描述  : 诊断报文发送处理
 输入参数  : N_UDDATA_REQ_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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

    if(pMsg->buf_len<8) //待发送的数据长度小于 8 单帧即可发送
    {
        TxTmp.Id=pMsg->sa+DIAG_BASE_ADRESS;
        TxTmp.Length=8;
        TxTmp.Data[0]=(uint8_t)pMsg->buf_len;
        NWL_CpyData(&(TxTmp.Data[1]),pMsg->buf,(uint8_t)pMsg->buf_len); 

        switch ( pMsg->sa )
        {
            case 0x50 :
                //单帧发送
                Send_Can_Message(TxTmp);
                UdsTxMsgCnt[UDS_Tx_750].MsgLthCnt = pMsg->buf_len;
                UdsMsg750Sending = 1;
                break;
            default:
                initTxMsgData(UDS_Tx_750);
                // 向应用层报错
                return FALSE;
        }
        
    }
    else //数据长度大于 等于 8时 需分段发送
    {
        switch ( pMsg->sa )
        {
            case 0x50 :
                TxNumTmp=UDS_Tx_750;
                break;
            default:
                initTxMsgData(UDS_Tx_750);

                // 向应用层确认
                return FALSE;
        }
              
        UdsTxMsgCnt[TxNumTmp].MsgLthCnt=0;
        UdsTxMsgCnt[TxNumTmp].SnCnt=0;
        UdsTxMsgCnt[TxNumTmp].RxOrTxCmpltFlg=0;
        SegSendFlag = FirstFrame; //置位连续帧形式发送标志

        Nw_UdsSendMsg(UDS_Tx_750);        
        
    }

    return TRUE;
}

/*****************************************************************************
 函 数 名  : NWL_RxFlowControlFrameProc
 功能描述  : 接收流控制帧处理
 输入参数  : CAN_BUF_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
        return FALSE;   //诊断仪发送的流控制帧的 DLC小于 3 仪表不回复连续帧   
    }
    
    FC_Stat = (_FS_STATUS_ENUM)(pMsg->Data[0]&0x0f);
    //if(NwlTime[UDS_750_STMIN].OnStatus == TimerOff)
    {
        /* 获取流控制状态 0-ContinueToSend  1-Wait  2-OverFlow  */
        if(g_CFSendFlag ==  0)
        {
            NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] = FC_Stat;
        }
        
        NwlTxBsData[Bstmp] = pMsg->Data[1];     //获取流控制帧帧中的  BS参数
        StminCmpData[Stmtmp] = pMsg->Data[2]+1;   //获取流控制帧中的 StMin 参数 
                                                  //( 柳汽反馈我们的 STmin 偏小 故 加 1)
        NwlTxBsCnt[Bstmp] = 0;                  //网络层发送 Bs计数清零                                          
        
        if(NwlTime[UDS_750_NBs].OnStatus == TimerOn)
        {
            NwlTime[UDS_750_NBs].OnStatus = TimerOff;    //关闭流控制帧等待计时器 N_Bs 
            NwlTime[UDS_750_NBs].CntData = 0;            //清零 N_Bs 计时器

        }  
        
        if(StminCmpData[Stmtmp] > N_CS_MAX) //如果接收方发送的流控制帧参数大于 N_CS_MAX 
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
                //接收到允许发送的流控制帧后开启 ST_MIN定时器
                g_CFSendFlag =  1;
                NwlTime[UDS_750_STMIN].OnStatus = TimerOn; 
                NwlTime[UDS_750_STMIN].CntData = 0;
            }
        }
        else if(NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] == Wait)
        {
            ResetDealStat();   //复位 UDS诊断服务正在处理状态 以接收新的诊断服务请求
            
            //NwlTime[UDS_740_NCr].OnStatus = TimerOn;
            //NwlTime[UDS_740_NCr].CntData = 0;
            initTxMsgData(UDS_Tx_750);
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
        else if(NwlFsStatus[UDS_RX_MSG_NUM+Txtmp] == OverFlow) /* 此流控制帧将迫使仪表中断多帧报文的发送 */
        {
            ResetDealStat();    //复位 UDS诊断服务正在处理状态 以接收新的诊断服务请求
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff; 
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
        else //还需更多处理
        {
            ResetDealStat();    //复位 UDS诊断服务正在处理状态 以接收新的诊断服务请求
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff;
            NwlTime[UDS_750_STMIN].CntData = 0;
        }
    }

    return TRUE;
}

/*****************************************************************************
 函 数 名  : NWL_TxFlowControlFrameProc
 功能描述  : UDS网络层发送流控制帧处理函数
 输入参数  : UDS_CAN_RX_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月23日星期三
    作    者   : xiaobo
    修改内容   : 新生成函数

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

    TxMsg.Data[0] |= (0X3 << 4);    //流控制帧
    TxMsg.Data[1] |= N_INIT_BS;     //设置仪表允许接收的 BS
    TxMsg.Data[2] |= N_INIT_STMIN;  //设置仪表允许接收的 STmin
    TxMsg.Length = 8;

    Send_Can_Message(TxMsg);  //调用 CAN驱动接口函数 发送流控制 CAN帧

    return TRUE;
}

#pragma CODE_SEG DEFAULT




