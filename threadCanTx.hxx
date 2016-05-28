#ifndef THREADCANTX_HXX
#define THREADCANTX_HXX

#include <QThread>
#include <QDebug>
#include <QLineEdit>
#include "canDevice.hxx"
#include "i15765NetLayer.hxx"

class TxThread : public QThread
{
    Q_OBJECT

public:
    TxThread(QObject * parent = 0);
    volatile bool m_bStopped;
    CanDevice *m_pCanDevice;
    I15765NetLayer *m_pI15765NetLayer;
    QLineEdit *m_pLineEditICID;

protected:
     void run();

private:

};

#endif // THREADCANTX_HXX
