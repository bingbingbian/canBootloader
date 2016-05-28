
#include "CanNetWorkLayer.h"
#include "DrvMSCAN.h"
#include "TxFuncs.h"
#include "i15765MsgProc.h"
#include "UdsApp.h"



uint8_t g_UdsNetRstFlag = 1;        /* UDS 网络层复位标志 */
NWL_TIM_STRU NwlTime[NWL_TIM_NUM];  /* 网络层定时器 */


/*****************************************************************************
 函 数 名  : Nw_CanMsgRxProc
 功能描述  : 网络层can报文接收处理
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
uint8_t Nw_CanMsgRxProc( void )
{
    uint8_t res = TRUE;    
    if(UDS_MSG.Flag == 1) //接到到诊断报文
    {   
        UDS_MSG.Flag = 0;
        res=NWL_udsRxMsg(&(UDS_MSG)); //诊断报文处理
    }
    return res;
}
/*****************************************************************************
 函 数 名  : NwFlowCheckAndDeal
 功能描述  : 流控制帧检测和处理
 输入参数  : UDS_CAN_RX_STRU *pMsg  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月22日星期二
    作    者   : fsfs
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t NwFlowCheckAndDeal(UDS_CAN_RX_STRU *pMsg)
{
    uint8_t res = TRUE;
    if((pMsg->Data[0]&0xF0) == 0x30)   //PCI 流控制帧
    {
        pMsg->Flag = 0;
        res=NWL_udsRxMsg(pMsg); //诊断报文处理
    }
    return res;
}
/*****************************************************************************
 函 数 名  : Nw_CanMsgTxProc
 功能描述  : 网络层CAN报文发送处理
 输入参数  : void  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2014年7月3日星期四
    作    者   : xiaobo
    修改内容   : 新生成函数

*****************************************************************************/
uint8_t Nw_CanMsgTxProc(void)
{
    uint8_t cnt;
    uint8_t res=NoFault;

    if(g_AppTxReqflg == 1) //应用层发送请求标志为1
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
 函 数 名  : NW_ErrCodeProc
 功能描述  : 网络层错误处理
 输入参数  : portBASE_TYPE code  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
 函 数 名  : vCanNetWorkLayerPartTask
 功能描述  : CAN网络层任务
 输入参数  : void *pvParameters  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

*****************************************************************************/
void vCanNetWorkLayerPartTask(void)
{        
    uint8_t res;   
    if(g_UdsNetRstFlag == 1)
    {
        /* 初始化 UDS buffer 包括 接收和发送buffer的 源地址 目标地址等 */
        g_UdsNetRstFlag = 0;
        udsMsgBufInit(); 
    }
    else
    {           
        res=Nw_CanMsgRxProc();  //网络层发送消息处理
        res=Nw_CanMsgTxProc();  //网络层接收消息处理
        
        if(res != NoFault)
        {
            NW_ErrCodeProc(res);
        }

    }
    
}

/*****************************************************************************
 函 数 名  : transDatatoConfirmData
 功能描述  : 接口数据转换
 输入参数  : N_UDDATA_CONFIRM_STRU *ConData  
             N_UDDATA_REQ_STRU *ReqData      
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
 函 数 名  : NwlTimerCnt
 功能描述  : 网络层计时器模块，需要在中断中周期调用
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
void NwlTimerCnt(void)
{
    uint8_t cycle; 
    
    if(UdsMsg750Sending  == 1)  //诊断响应报文正在发送 
    {
        if(CANTFLG_TXE2 == 1)   //当前 CAN帧发送成功
        {
            UdsMsg750TxOk = 1;  
            UdsMsg750Sending = 0;   
        }
        
    }
    
    if(UdsMsg750TxOk == 1)
    {
        /* 709累计字节数大于等于连续帧总字节数 且 当前 709队列中消息数为零 */
        if((UdsTxMsgCnt[UDS_Tx_750].MsgLthCnt >= UdsTxMsgBuf[UDS_Tx_750].buf_len))//&&(uxQueueMessagesWaitingFromISR(Uds709DivQueue)==0))
        {
            ResetDealStat();    //复位 UDS诊断服务正在处理状态 以接收新的诊断服务请求
            UdsTxMsgCnt[UDS_Tx_750].RxOrTxCmpltFlg = 1;  /* 置750 发送或则接收标志为1 */
            NwlTime[UDS_750_STMIN].OnStatus = TimerOff;  /* 关闭750 StMin定时器  停止发送 */
        }
        NwlTime[UDS_750_STMIN].CntData = 0;   /* 750计时器清零 */
        UdsMsg750TxOk = 0;    /* 750发送成功标志清零 */
    }    

    for ( cycle = 0 ; cycle < NWL_TIM_NUM; cycle++ )
    {
        if(NwlTime[cycle].OnStatus == TimerOn)
        {
            NwlTime[cycle].CntData++;   /* 如果定时器为开启状态 定时变量自加 */
        }
        else
        {
            NwlTime[cycle].CntData = 0;
        }
    }

    /* 709流控制状态为继续发送 且 709 StMin到点或则超时 且 (还未发送到709所指定流控制块大小
       或者709流控制块大小为0 ) */
    if((NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750] == ContinueToSend)
        &&(NwlTime[UDS_750_STMIN].CntData >= StminCmpData[UDS_750_STMIN])
        &&((NwlTxBsCnt[UDS_BS_750]<NwlTxBsData[UDS_BS_750])||(NwlTxBsData[UDS_BS_750] == 0)))
    {
        if(TRUE == Nw_UdsSendMsg(UDS_Tx_750)) //发送一帧连续帧
        {
        
            NwlTime[UDS_750_STMIN].CntData = 0;    /* 清零 StMin定时器 */           
            NwlTxBsCnt[UDS_BS_750]++;   //发送 Bs BlockSize 计数加 1
        }
    }
    else
    {
        /* 750发送的帧已达块大小数 或则 网络层750的流控制状态为等待(发送完首帧后) */
        if((NwlTxBsCnt[UDS_Tx_750] >= NwlTxBsData[UDS_Tx_750])  \
            ||(NwlFsStatus[UDS_RX_MSG_NUM+UDS_Tx_750] == Wait)) 
        {
            if(NwlTime[UDS_750_NBs].CntData > N_BS_MAX_TIME)
            {
                //接收流控制帧等待超时
                NwlTime[UDS_750_NBs].OnStatus = TimerOff;
                NwlTime[UDS_750_NBs].CntData = 0;
               
                initTxMsgData(UDS_Tx_750);
                ResetDealStat();    //复位 UDS诊断服务正在处理状态 以接收新的诊断服务请求
                /* 向应用层报告 */
            }
            if(NwlTxBsData[UDS_Tx_750] != 0)
            {
                if(NwlTime[UDS_750_STMIN].OnStatus == TimerOn)
                {
                    NwlTime[UDS_750_STMIN].OnStatus = TimerOff;
                    NwlTime[UDS_750_STMIN].CntData = 0;    /* 清零 StMin定时器 */ 
                }
            }
        }
        else
        {
            if(NwlTime[UDS_750_NCs].CntData > N_CS_MAX)//  N_CR_MAX_TIME N_CS由应用层管理
            {
              
            }
        }
    }
    
    if(NwlTime[UDS_740_NCr].OnStatus == TimerOn)
    {
        if(NwlTime[UDS_740_NCr].CntData > N_CR_MAX_TIME)    //仪表接收 740 报文连续帧超时
        {
            initRxMsgData(UDS_RX_740 , N_TIMEOUT_Cr);
            //initTxMsgData(UDS_Tx_750);
            NwlTime[UDS_740_NCr].OnStatus = TimerOff;
        }
    }

}

    
