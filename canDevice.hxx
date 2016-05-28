#ifndef CANDEVICE_HXX
#define CANDEVICE_HXX

#include <QObject>
#include <QLibrary>
#include <QDebug>
#include <QString>
#include "icsneo40DLLAPI.hxx"

class CanDevice : public QObject
{
    Q_OBJECT

public:
    CanDevice(QObject * parent = 0);
    bool m_isOpened;
    int m_hObject;
    int m_iOpenDeviceType;
    QString m_sOpenDeviceType;
    int m_iNumberOfErrors;
    int m_iNumberOfMessages;
    icsSpyMessage m_stMessages[20000];
    icsSpyMessage m_txMessages;
    int m_iNetworkID;

    int setBitRate(int BitRate, int NetworkID);
    int waitForRxMessagesWithTimeOut(unsigned int iTimeOut);
    int getMessages();
    int txMessages();

private:
    FINDNEODEVICES icsneoFindNeoDevices;
    OPENNEODEVICE icsneoOpenNeoDevice;
    CLOSEPORT icsneoClosePort;
    GETMESSAGES icsneoGetMessages;
    TXMESSAGES icsneoTxMessages;
    SETBITRATE icsneoSetBitRate;
    WAITFORRXMSGS icsneoWaitForRxMessagesWithTimeOut;

signals:
    void deviceStatChanged();

public slots:
    void openDevice(void);
    void closeDevice(void);
};

#endif // CANDEVICE_HXX
