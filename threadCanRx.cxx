#include "threadCanRx.hxx"


RxThread::RxThread(QObject * parent) :
    QThread(parent)
{
    m_bStopped = false;
    m_pCanDevice = NULL;
    m_pI15765NetLayer = NULL;
    m_pLineEditPCID = NULL;
}

void RxThread::run()
{
    int iRet = 0;
    int iCnt = 0;
    int iDataByteCnt = 0;
    QString str;

    while(!m_bStopped)
    {
        sleep(1);
        qDebug() << "Rx Thread";

        iRet = this->m_pCanDevice->waitForRxMessagesWithTimeOut(5000);
        if(iRet == 0)
        {
            //m_bStopped = true;
            qDebug() << tr("接收报文超时");
            //
            this->exit();
            return;
        }
        iRet = this->m_pCanDevice->getMessages();
        if(iRet == 0)
        {
            qDebug() << tr("获取报文故障");
            this->exit();
            return;
        }
        for(iCnt=0; iCnt < m_pCanDevice->m_iNumberOfMessages; iCnt++)   //Loop to acquire all of the messages in the buffer
        {
            str.clear();   //Clear string
            iDataByteCnt = m_pCanDevice->m_stMessages[iCnt].NumberBytesData;

            //lResult = icsneoGetTimeStampForMsg(m_hObject, &stMessages[lCount - 1], &dTime);

            str = "Length : " + QString::number(iDataByteCnt) + " ";  //Format time and add to output string
            if((m_pCanDevice->m_stMessages[iCnt].StatusBitField & 2)!=0)   //See if message is RX or TX
            {
                str += "Tx Message ";
            }
            else
            {
                str += "Rx Message ";
            }

            switch(m_pCanDevice->m_stMessages[iCnt].NetworkID)
            {
                case NETID_HSCAN:
                    str += "HSCAN";
                    break;
                case NETID_MSCAN:
                    str += "MSCAN";
                    break;
                case NETID_SWCAN:
                    str += "SWCAN";
                    break;
                case NETID_LSFTCAN:
                    str += "LSFTCAN";
                    break;
                case NETID_FORDSCP:
                    str += "FORD SCP";
                    break;
                case NETID_J1708:
                    str += "J1708";
                    break;
                case NETID_AUX:
                    str += "AUX";
                    break;
                case NETID_JVPW:
                    str += "J1850 VPW";
                    break;
                case NETID_ISO:
                    str += "ISO/UART";
                    break;
                default:
                    str += "Unknown";
                    break;
            }

            if(m_pCanDevice->m_stMessages[iCnt].Protocol == SPY_PROTOCOL_CAN)
            {
                //sListString = sListString + stMessages[lCount - 1].NetworkID;
                str += " ArbID-";  //Create output string with ArbID and Data Bytes
                str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].ArbIDOrHeader) + " Data-";
                if(iDataByteCnt>=1) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[0]) + " ";
                if(iDataByteCnt>=2) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[1]) + " ";
                if(iDataByteCnt>=3) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[2]) + " ";
                if(iDataByteCnt>=4) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[3]) + " ";
                if(iDataByteCnt>=5) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[4]) + " ";
                if(iDataByteCnt>=6) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[5]) + " ";
                if(iDataByteCnt>=7) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[6]) + " ";
                if(iDataByteCnt>=8) str = str + QString::number(m_pCanDevice->m_stMessages[iCnt].Data[7]) + " ";
            }            
        }

        //接收报文递交网络层处理
        iCnt = 0;
        while(iCnt < m_pCanDevice->m_iNumberOfMessages)
        {
            if(m_pI15765NetLayer->isRxBufEmpty())
            {
                if((m_pCanDevice->m_stMessages[iCnt].StatusBitField & 2)!=0)   //See if message is RX or TX
                {
                    //"Tx Message ";
                    iCnt ++;
                }
                else
                {
                    //"Rx Message ";
                    udsMsg.ID = m_pCanDevice->m_stMessages[iCnt].ArbIDOrHeader;
                    udsMsg.dataLen = m_pCanDevice->m_stMessages[iCnt].NumberBytesData;
                    udsMsg.dataBuf[0] = m_pCanDevice->m_stMessages[iCnt].Data[0];
                    udsMsg.dataBuf[1] = m_pCanDevice->m_stMessages[iCnt].Data[1];
                    udsMsg.dataBuf[2] = m_pCanDevice->m_stMessages[iCnt].Data[2];
                    udsMsg.dataBuf[3] = m_pCanDevice->m_stMessages[iCnt].Data[3];
                    udsMsg.dataBuf[4] = m_pCanDevice->m_stMessages[iCnt].Data[4];
                    udsMsg.dataBuf[5] = m_pCanDevice->m_stMessages[iCnt].Data[5];
                    udsMsg.dataBuf[6] = m_pCanDevice->m_stMessages[iCnt].Data[6];
                    udsMsg.dataBuf[7] = m_pCanDevice->m_stMessages[iCnt].Data[7];
                    m_pI15765NetLayer->getUdsMsg(&udsMsg);
                    m_pI15765NetLayer->dealUdsMsg();
                    iCnt ++;
                }
            }
        }
    }
}
