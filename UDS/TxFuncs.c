/******************************************************************************

                  版权所有 (C), 成都天兴仪表股份有限公司

 ******************************************************************************
  文 件 名   : TxFuncs.c
  版 本 号   : 初稿
  作    者   : Tan Zhihua
  生成日期   : 2013年1月3日
  最近修改   :
  功能描述   : 发送函数模块
  函数列表   :
              i15765_tx_app
  修改历史   :
  1.日    期   : 2013年1月3日
    作    者   : Tan Zhihua
    修改内容   : 创建文件

******************************************************************************/
#include "uds.h"
#include "CanNetWorkLayer.h"
#include "TxFuncs.h"

N_UDDATA_REQ_STRU g_AppTxReq;   //应用层请求原语
uint8_t g_AppBuf[MAX_UDS_TX_BUF_BYTES];   //应用层发送调用 buffer
uint8_t g_AppTxReqflg;          //应用层请求原语发送标志

/*****************************************************************************
 函 数 名  : i15765_tx_app
 功能描述  : 15765报文发送处理
 输入参数  : i15765_t*msg                
             I15765_TX_STATUS *txStatus  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2013年1月28日
    作    者   : Tan Zhihua
    修改内容   : 新生成函数

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
    
    g_AppTxReqflg = 1; //置位应用层请求原语发送标志 通知网络层
    
    *txStatus=I15765_SENT;//该处应作判断 后续完善
    
}


