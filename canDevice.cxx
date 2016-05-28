#include "canDevice.hxx"

CanDevice::CanDevice(QObject *parent) :
    QObject(parent)
{    
    this->m_isOpened = false;
    this->m_hObject = 0;
    this->m_iOpenDeviceType = 0;
    this->m_sOpenDeviceType.clear();

    icsneoFindNeoDevices = NULL;
    icsneoOpenNeoDevice = NULL;
    icsneoClosePort = NULL;
    icsneoGetMessages = NULL;
    icsneoTxMessages = NULL;
    icsneoSetBitRate = NULL;
    icsneoWaitForRxMessagesWithTimeOut = NULL;

    QLibrary mylib("icsneo40.dll");
    if(mylib.load())
    {
        icsneoFindNeoDevices = (FINDNEODEVICES)mylib.resolve("icsneoFindNeoDevices");
        icsneoOpenNeoDevice = (OPENNEODEVICE)mylib.resolve("icsneoOpenNeoDevice");
        icsneoClosePort = (CLOSEPORT)mylib.resolve("icsneoClosePort");
        icsneoGetMessages = (GETMESSAGES)mylib.resolve("icsneoGetMessages");
        icsneoTxMessages = (TXMESSAGES)mylib.resolve("icsneoTxMessages");
        icsneoSetBitRate = (SETBITRATE)mylib.resolve("icsneoSetBitRate");
        icsneoWaitForRxMessagesWithTimeOut = (WAITFORRXMSGS)mylib.resolve("icsneoWaitForRxMessagesWithTimeOut");

        if(!icsneoFindNeoDevices || !icsneoOpenNeoDevice || !icsneoClosePort ||
           !icsneoGetMessages || !icsneoTxMessages || !icsneoSetBitRate ||
           !icsneoWaitForRxMessagesWithTimeOut)
        {
            qDebug() << tr("����'icsneo40.dll'�еĺ���ʧ��");
            return;
        }
    }
    else
    {
        qDebug() << tr("����'icsneo40.dll'ʧ��");
    }
}

void CanDevice::openDevice()
{
    int iResult;
    NeoDevice ndNeoToOpen;
    int iNumberOfDevices;

    if(this->m_isOpened)
    {
        return;
    }

    iNumberOfDevices = 1;
    iResult = icsneoFindNeoDevices(65535, &ndNeoToOpen, &iNumberOfDevices);
    if(iResult == false)
    {
        qDebug() << tr("����CAN�豸ʱ����");
        return;
    }
    else
    {
        qDebug() << tr("����CAN�豸���");
    }

    if(iNumberOfDevices<1)
    {
        qDebug() << tr("δ�ҵ�����CAN�豸");
        return;
    }

    m_hObject = 0;
    iResult = icsneoOpenNeoDevice(&ndNeoToOpen, &m_hObject, NULL,1,0);
    if(iResult==false)
    {
        qDebug() << tr("��CAN�豸�˿ڹ���");
        return;
    }
    qDebug() << tr("�ɹ���CAN�豸�˿�");
    m_iOpenDeviceType = ndNeoToOpen.DeviceType;

    //ui->listWidget->takeItem(0);
    switch(m_iOpenDeviceType)
    {
        case 1:
            m_sOpenDeviceType = "neoVI Blue SN " + QString::number(ndNeoToOpen.SerialNumber);
            break;
        case 4:
            m_sOpenDeviceType = "ValueCAN 2 SN " + QString::number(ndNeoToOpen.SerialNumber);
            break;
        case 8:
            m_sOpenDeviceType = "neoVI FIRE SN " + QString::number(ndNeoToOpen.SerialNumber);
            break;
        case 16:
            m_sOpenDeviceType = "ValueCAN 3 SN " + QString::number(ndNeoToOpen.SerialNumber);
            break;
        default:
            m_sOpenDeviceType = "Unknown neoVI SN " + QString::number(ndNeoToOpen.SerialNumber);
    }
    qDebug() << m_sOpenDeviceType;

    this->m_isOpened = true;
    emit deviceStatChanged();
}

void CanDevice::closeDevice()
{
    int iResult;
    int iNumberOfErrors;

    if(!this->m_isOpened)
    {
        qDebug() << tr("��δ���豸");
        return;
    }
    iResult = icsneoClosePort(m_hObject,&iNumberOfErrors);
    if(iResult==0)
    {
        qDebug() << tr("�ر�CAN�豸�˿�ʱ����");
        return;
    }
    else
    {
        qDebug() << tr("CAN�豸�رճɹ�");
    }

    this->m_isOpened = false;
    emit deviceStatChanged();
}

int CanDevice::setBitRate(int BitRate, int NetworkID)
{
    int iResult;

    if(!this->m_isOpened)
    {
        qDebug() << tr("neoVI not opened");
        return 0;
    }    

    //Set the bit rate
    iResult = icsneoSetBitRate(m_hObject, BitRate, NetworkID);
    if (iResult == 0)
    {
        qDebug() << tr("Problem setting bit rate");
        return 0;
    }
    else
    {
        qDebug() << tr("Bit rate Set");
        return 1;
    }
}

int CanDevice::waitForRxMessagesWithTimeOut(unsigned int iTimeOut)
{
    int iRet;

    iRet = icsneoWaitForRxMessagesWithTimeOut(m_hObject, iTimeOut);
    return iRet;
}

int CanDevice::getMessages()
{
    int iRet;

    iRet = icsneoGetMessages(this->m_hObject, this->m_stMessages, &this->m_iNumberOfMessages, &this->m_iNumberOfErrors);
    return iRet;
}

int CanDevice::txMessages()
{
    int iRet;

    iRet = icsneoTxMessages(this->m_hObject, &this->m_txMessages, this->m_iNetworkID, 1);
    return iRet;
}
