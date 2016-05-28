#ifndef THREADCANRX_HXX
#define THREADCANRX_HXX

#include <QThread>
#include <QDebug>
#include <QLineEdit>
#include "canDevice.hxx"
#include "i15765NetLayer.hxx"

class RxThread : public QThread
{    
    Q_OBJECT

public:
    RxThread(QObject * parent = 0);
    volatile bool m_bStopped;
    CanDevice *m_pCanDevice;
    I15765NetLayer *m_pI15765NetLayer;
    QLineEdit *m_pLineEditPCID;

protected:
    void run();    

private:
    UDS_MSG udsMsg;    //CanDevice与I15765NetLayer的接收报文接口
    UDS_MSG udsTxMsg;   //CanDevice与I15765NetLayer的发送报文接口

signals:

private slots:
};

#endif // THREADCANRX_HXX
