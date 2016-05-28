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

    if(CheckDealStat() == true)    //����Ϸ������ڴ������Ե�ǰ�µ�����
    {
        return false;
    }

    tmp = pMsg->dataBuf[0]&0x0f;   //��ȡSF_DLC
    if((tmp >= 8)||(tmp == 0)) //��֡��Ч���ݳ��Ȳ��ܴ���7�Ҳ���Ϊ0
    {
        return false;
    }
    else if(pMsg->dataLen < (tmp+1))  //CAN_DLC��ӦС��SF_DLC+1 ( sizeof(N_PCI) + SF_DLC)
    {
        return false;
    }

    UdsRxMsgBuf[MsgChannel].buf_len = tmp;    //���ý���buf��Ч�������ݳ���
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;        //����buf֡�������
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;    //����buf����֡��������
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 0;    //����buf����֡�ѽ����ܳ�������
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = true;   //���ջ��߷��ͳɹ���־��true

    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->dataBuf[1]),tmp); //������֡��Ч����֡������buffer
    UdsRxMsgBuf[MsgChannel].result = N_OK;

    //��Ӧ�ò��ṩ���ճɹ�ָʾ
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
                return false;  //����Ѱַ��֧�ֶ�֡����������֡
            }
            break;
        default:
            return false;
    }

    if(CheckDealStat() == true)    //����Ϸ������ڴ������Ե�ǰ�µ�����
    {
        return false;
    }

    tmp = (uint16_t)((pMsg->dataBuf[0]&0x0f)<<8)+(pMsg->dataBuf[1]); //FF֡���ݳ��Ƚ���
    if(tmp<8)  //����֡�ĵ�һ֡������Ӧ����8
    {
        return false;
    }
    else if(tmp > MAX_UDS_TX_BUF_BYTES)  //��������շ�������
    {
        initRxMsgData(MsgChannel , N_WRONG_SN);

        NwlFsStatus[MsgChannel] = OverFlow;
        NWL_TxFlowControlFrameProc(pMsg);   //����������״̬ΪOverFlow��������֡

        return false;
    }

    //�������ݷ��ͽ��ո�������
    NwlFsStatus[MsgChannel] = ContinueToSend;
    NWL_TxFlowControlFrameProc(pMsg); //����������֡

    UdsRxMsgBuf[MsgChannel].buf_len = tmp;
    UdsRxMsgCnt[MsgChannel].SnCnt = 0;
    UdsRxMsgCnt[MsgChannel].SnAddData = 0;
    UdsRxMsgCnt[MsgChannel].RxOrTxCmpltFlg = 0;
    UdsRxMsgCnt[MsgChannel].MsgLthCnt = 6;  //��֡����6���ֽڵ���Ч����
    //����uds����buffer
    udsclrMsgBufData(UdsRxMsgBuf[MsgChannel].buf, MAX_UDS_TX_BUF_BYTES);
    NWL_CpyData(UdsRxMsgBuf[MsgChannel].buf,&(pMsg->dataBuf[2]),6); //������֡�е���Ч����

    //������������֡�ȴ�ʱ�䶨ʱ��
    /*
    if(MsgChannel == UDS_RX_740)
    {
        NwlTime[UDS_740_NCr].OnStatus = TimerOn;
        NwlTime[UDS_740_NCr].CntData = 0; //���յ����ݣ�����NCr
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

    TxMsg.dataBuf[0] |= (0X3 << 4);    //������֡����
    TxMsg.dataBuf[1] |= N_INIT_BS;     //�����Ǳ�������յ�BS
    TxMsg.dataBuf[2] |= N_INIT_STMIN;  //�����Ǳ�������յ�STmin
    TxMsg.dataLen = 8;

    emit Send_Can_Message(TxMsg);  //����CAN����

    return true;
}
