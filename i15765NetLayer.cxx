#include "i15765NetLayer.hxx"

I15765NetLayer :: I15765NetLayer(QObject *parent) :
    QObject(parent)
{
    m_UdsMsg.bRxOk = false;
}

bool I15765NetLayer::isRxBufEmpty()
{
    return !m_UdsMsg.bRxOk;
}

void I15765NetLayer::getUdsMsg(UDS_MSG *pUdsMsg)
{
    if(pUdsMsg == NULL)
    {
        qDebug() << tr("Error: in 'void I15765NetLayer::getUdsMsg(UDS_MSG *pUdsMsg)' UDS_MSG *pUdsMsg==NULL");
        return;
    }
    m_UdsMsg.bRxOk = pUdsMsg->bRxOk;
    m_UdsMsg.ID = pUdsMsg->ID;
    m_UdsMsg.dataLen = pUdsMsg->dataLen;
    //API_memSet(&m_UdsMsg.dataBuf, 0, 20);
    API_memCpy(m_UdsMsg.dataBuf, pUdsMsg->dataBuf, pUdsMsg->dataLen);
}

void I15765NetLayer::dealUdsMsg()
{
    if(m_UdsMsg.bRxOk)
    {
        m_UdsMsg.bRxOk = false;
        NWL_udsRxMsg(&(m_UdsMsg));
    }
}


////////////////////////////////////////////////////////////////////////////////////////

uint8_t I15765NetLayer::API_memCpy(uint8_t* pDst, const uint8_t* pSrc, uint32_t bytes)
{
    uint32_t i;
    if((pDst == NULL) || (pSrc == NULL))
    {
        return FALSE;
    }
    else
    {
        for(i=0; i<bytes; i++)
        {
            *pDst++ = *pSrc++;
        }
    }
    return TRUE;
}


uint8_t I15765NetLayer::API_memSet(uint8_t* pDst, uint8_t value, uint32_t bytes)
{
    uint32_t i;
    if(pDst == NULL)
    {
        return FALSE;
    }
    else
    {
        for(i=0; i<bytes; i++)
        {
            *pDst++ = value;
        }
    }
    return TRUE;
}

uint8_t I15765NetLayer::Nw_CanMsgRxProc()
{
    uint8_t res = true;

    if(m_UdsMsg.bRxOk == 1)
    {
        m_UdsMsg.bRxOk = 0;
        res=NWL_udsRxMsg(&m_UdsMsg);
    }
    return res;
}

uint8_t I15765NetLayer::NWL_udsRxMsg(UDS_MSG *pMsg)
{
    uint8_t res = true;

    switch(pMsg->dataBuf[0]&0xf0)
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
            res=false;
    }

    return res;
}

uint8_t I15765NetLayer::NWL_SFMsgProc(UDS_MSG *pMsg)
{
    uint8_t res=true;
    uint8_t MsgChannel=0,tmp=0;

    switch(pMsg->ID)
    {
        case 0x740 :
            MsgChannel = UDS_RX_740;
            break;
        case 0x7df :
            MsgChannel = UDS_RX_7DF;
            break;
        default:
            return false;
    }

    if(CheckDealStat() == true)    //有诊断服务正在处理，忽略当前新的请求
    {
        return false;
    }

    tmp = pMsg->dataBuf[0]&0x0f;   //提取SF_DLC
    if((tmp >= 8)||(tmp == 0)) //单帧有效数据长度不能大于7且不能为0
    {
        return false;
    }
    else if(pMsg->dataLen < (tmp+1))  //CAN_DLC不应小于SF_DLC+1 ( sizeof(N_PCI) + SF_DLC)
    {
        return false;
    }

    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    //设置接收buf有效接收数据长度
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;        //接收buf帧序号清零
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;    //接收buf连续帧计数清零
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 0;    //接收buf连续帧已接收总长度清零
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = true;   //接收或者发送成功标志置true

    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->dataBuf[1]),tmp); //拷贝单帧有效数据帧到接收buffer
    UdsRxMsgBuf[MsgChannel].result = N_OK;

    //向应用层提供接收成功指示
    //g_RxChannel = MsgChannel;
    m_bRxOK = 1;
    //start RxQueue Timer

    return res;
}

uint8_t I15765NetLayer::NWL_FFMsgProc(UDS_MSG *pMsg)
{
    uint8_t MsgChannel=0;
    uint16_t tmp=0;

    switch(pMsg->ID)
    {
        case 0x740:
            {
                MsgChannel=UDS_RX_740;
            }
            break;
        case 0x7df:
            {
                //MsgChannel=UDS_RX_7DF;
                return false;  //功能寻址不支持多帧，不处理首帧
            }
            break;
        default:
            return false;
    }

    if(CheckDealStat() == true)    //有诊断服务正在处理，忽略当前新的请求
    {
        return false;
    }

    tmp = (uint16_t)((pMsg->dataBuf[0]&0x0f)<<8)+(pMsg->dataBuf[1]); //FF帧数据长度解析
    if(tmp<8)  //连续帧的第一帧长度域应大于8
    {
        return false;
    }
    else if(tmp > MAX_UDS_TX_BUF_BYTES)  //超过最大收发处理缓存
    {
        initRxMsgData(MsgChannel , N_WRONG_SN);

        NwlFsStatus[MsgChannel] = OverFlow;
        NWL_TxFlowControlFrameProc(pMsg);   //发送流控制状态为OverFlow的流控制帧

        return false;
    }

    //连续数据发送接收更新正常
    NwlFsStatus[MsgChannel] = ContinueToSend;
    NWL_TxFlowControlFrameProc(pMsg); //发送流控制帧

    UdsRxMsgBuf[MsgChannel].buf_len = tmp;
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 0;
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 6;  //首帧中有6个字节的有效数据
    //清零uds接收buffer
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->dataBuf[2]),6); //拷贝首帧中的有效数据

    //开启接收连续帧等待时间定时器
    /*
    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].OnStatus = TimerOn;
        NwlTime[UDS_740_NCr].CntData = 0; //接收到数据，清零NCr
    }

    if(NwlTime[UDS_750_STMIN].OnStatus == TimerOn)
    {
        NwlTime[UDS_750_STMIN].OnStatus = TimerOff;
        NwlTime[UDS_750_STMIN].CntData = 0;
    }*/
    return true;
}

uint8_t I15765NetLayer::NWL_CFMsgProc(UDS_MSG *pMsg)
{
    ;
}

uint8_t I15765NetLayer::NWL_FCFMsgProc(UDS_MSG *pMsg)
{
    ;
}

uint8_t I15765NetLayer::CheckDealStat()
{
    return false;
}

void I15765NetLayer::udsclrMsgBufData(uint8_t *Point,uint8_t MsgLength)
{
    uint16_t cycle;

    if(Point == NULL) return;

    for ( cycle = 0 ; cycle < MsgLength; cycle++ )
    {
        Point[cycle]=0;
    }
}

void I15765NetLayer::NWL_CpyData(uint8_t *Dr,uint8_t *Sr,uint8_t MsgLength)
{
    uint8_t cycle;

    if((Dr == NULL) || (Sr == NULL)) return;

    for ( cycle = 0 ; cycle < MsgLength ; cycle++ )
    {
        Dr[cycle] = Sr[cycle];
    }
}

void I15765NetLayer::initRxMsgData(uint8_t MsgChannel,N_US_RESULT_ENUM result)
{
    UdsRxMsgBuf[MsgChannel].buf_len=0;
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    UdsRxMsgCnt[MsgChannel].MsgLthCnt=0;
    UdsRxMsgCnt[MsgChannel].SnCnt=0;
    UdsRxMsgCnt[MsgChannel].SnAddData=0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg=0;
    UdsRxMsgBuf[MsgChannel].result = result;
}

uint8_t I15765NetLayer::NWL_TxFlowControlFrameProc(UDS_MSG *pMsg)
{
    UDS_MSG TxMsg;
    uint8_t cycle;

    for ( cycle = 0 ; cycle < 8 ; cycle++ )
    {
        TxMsg.dataBuf[cycle]=0;
    }

    switch (pMsg->ID)
    {
        case 0x740 :
            TxMsg.ID = 0x750;
            TxMsg.dataBuf[0] = (uint8_t)NwlFsStatus[UDS_RX_740];
            break;
        case 0x7df :
            TxMsg.ID = 0x750;
            TxMsg.dataBuf[0] = (uint8_t)NwlFsStatus[UDS_RX_7DF];
            break;
        default:
            return false;
    }

    TxMsg.dataBuf[0] |= (0X3 << 4);    //流控制帧类型
    TxMsg.dataBuf[1] |= N_INIT_BS;     //设置仪表允许接收的BS
    TxMsg.dataBuf[2] |= N_INIT_STMIN;  //设置仪表允许接收的STmin
    TxMsg.dataLen = 8;

    emit Send_Can_Message(TxMsg);  //发送CAN报文

    return true;
}
