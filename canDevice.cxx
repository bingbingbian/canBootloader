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
            qDebug() << tr("加载'icsneo40.dll'中的函数失败");
            return;
        }
    }
    else
    {
        qDebug() << tr("加载'icsneo40.dll'失败");
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
        qDebug() << tr("查找CAN设备时故障");
        return;
    }
    else
    {
        qDebug() << tr("查找CAN设备完成");
    }

    if(iNumberOfDevices<1)
    {
        qDebug() << tr("未找到可用CAN设备");
        return;
    }

    m_hObject = 0;
    iResult = icsneoOpenNeoDevice(&ndNeoToOpen, &m_hObject, NULL,1,0);
    if(iResult==false)
    {
        qDebug() << tr("打开CAN设备端口故障");
        return;
    }
    qDebug() << tr("成功打开CAN设备端口");
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
        qDebug() << tr("尚未打开设备");
        return;
    }
    iResult = icsneoClosePort(m_hObject,&iNumberOfErrors);
    if(iResult==0)
    {
        qDebug() << tr("关闭CAN设备端口时故障");
        return;
    }
    else
    {
        qDebug() << tr("CAN设备关闭成功");
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
