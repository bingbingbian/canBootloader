#ifndef WIDGET_HXX
#define WIDGET_HXX

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "canDevice.hxx"
#include "threadCanRx.hxx"
#include "threadCanTx.hxx"
#include "i15765NetLayer.hxx"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    
private:
    Ui::Widget *ui;
    CanDevice *m_pCanDevice;
    I15765NetLayer *m_pI15765NetLayer;
    RxThread *m_threadCanRx;
    TxThread *m_threadCanTx;

private slots:
    void openFile(void);
    void start(void);
    void startDebug1();
    void onDeviceStatChanged();
    void onSetBitRate();
    void onRxThreadFinished();
    void onTxThreadFinished();
};

#endif // WIDGET_HXX
