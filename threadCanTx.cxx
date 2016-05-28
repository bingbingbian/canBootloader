#include "threadCanTx.hxx"


TxThread::TxThread(QObject * parent) :
    QThread(parent)
{
    m_bStopped = false;
    m_pCanDevice = NULL;
    m_pI15765NetLayer = NULL;
    m_pLineEditICID = NULL;
}

void TxThread::run()
{
    long lResult;
    QString str;

    while(!m_bStopped)
    {
        sleep(1);
        qDebug() << "Tx Thread";

        // Has the uset open neoVI yet?
        if(!this->m_pCanDevice->m_isOpened)
        {
            qDebug() << tr("txThread neoVI not opened");
            //
            this->exit();
            return;
        }

        m_pCanDevice->m_iNetworkID = NETID_HSCAN;
        m_pCanDevice->m_txMessages.StatusBitField = 0;	// not extended or remote frame
        m_pCanDevice->m_txMessages.StatusBitField2 = 0;

        str = this->m_pLineEditICID->text();
        m_pCanDevice->m_txMessages.ArbIDOrHeader = str.toLong(0, 16); // The ArbID
        m_pCanDevice->m_txMessages.NumberBytesData = 8; // The number of Data Bytes

        // Load all of the data bytes in the structure
        m_pCanDevice->m_txMessages.Data[0] = 0x0;
        m_pCanDevice->m_txMessages.Data[1] = 0x1;
        m_pCanDevice->m_txMessages.Data[2] = 0x2;
        m_pCanDevice->m_txMessages.Data[3] = 0x3;
        m_pCanDevice->m_txMessages.Data[4] = 0x4;
        m_pCanDevice->m_txMessages.Data[5] = 0x5;
        m_pCanDevice->m_txMessages.Data[6] = 0x6;
        m_pCanDevice->m_txMessages.Data[7] = 0x7;

        // Transmit the assembled message
        lResult =  m_pCanDevice->txMessages();
        if(lResult==0)
        {
            qDebug() << tr("Problem Transmitting Message");
            this->exit();
            return;
        }
        sleep(1);
        m_pCanDevice->m_txMessages.Data[0] = 0x7;
        m_pCanDevice->m_txMessages.Data[1] = 0x6;
        m_pCanDevice->m_txMessages.Data[2] = 0x5;
        m_pCanDevice->m_txMessages.Data[3] = 0x4;
        m_pCanDevice->m_txMessages.Data[4] = 0x3;
        m_pCanDevice->m_txMessages.Data[5] = 0x2;
        m_pCanDevice->m_txMessages.Data[6] = 0x1;
        m_pCanDevice->m_txMessages.Data[7] = 0x0;
        lResult =  m_pCanDevice->txMessages();
        if(lResult==0)
        {
            qDebug() << tr("Problem Transmitting Message");
            this->exit();
            return;
        }
    }
}
