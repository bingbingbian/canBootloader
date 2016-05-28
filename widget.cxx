#include "widget.hxx"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->listWidget->addItem(tr("尚未连接CAN设备"));
    ui->lineEdit_PCID->setText("123");
    ui->lineEdit_ICNID->setText("456");
    setWindowTitle("canBootLoader");
    setFixedSize(this->width(), this->height());

    m_pCanDevice = new CanDevice(this);
    m_pI15765NetLayer = new I15765NetLayer(this);

    connect(ui->pushButton_connect, SIGNAL(clicked(bool)), m_pCanDevice, SLOT(openDevice()));
    connect(ui->pushButton_disconnect, SIGNAL(clicked(bool)), m_pCanDevice, SLOT(closeDevice()));
    connect(ui->pushButton_setBitRate, SIGNAL(clicked(bool)), this, SLOT(onSetBitRate()));
    connect(m_pCanDevice,SIGNAL(deviceStatChanged()), this, SLOT(onDeviceStatChanged()));

    connect(ui->pushButton_openFile, SIGNAL(clicked(bool)), this, SLOT(openFile()));
    connect(ui->pushButton_start, SIGNAL(clicked(bool)),this, SLOT(start()));
    connect(ui->pushButton_debug1, SIGNAL(clicked(bool)),this,SLOT(startDebug1()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::openFile()
{
    QString directory = QFileDialog::getOpenFileName(this, tr("Open Files"), QDir::currentPath());
    directory.replace("/", "\\");

    ui->lineEdit_directory->setText(directory);
}

void Widget::start()
{
    ui->pushButton_openFile->setEnabled(false);
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_setBitRate->setEnabled(false);
    ui->pushButton_start->setEnabled(false);
    ui->comboBox_BitRate->setEnabled(false);
    ui->comboBox_CanChannel->setEnabled(false);
    ui->lineEdit_ICNID->setEnabled(false);
    ui->lineEdit_PCID->setEnabled(false);

    m_threadCanRx = new RxThread(this);
    connect(m_threadCanRx, SIGNAL(finished()),this,SLOT(onRxThreadFinished()));
    m_threadCanRx->m_pCanDevice = this->m_pCanDevice;
    m_threadCanRx->m_pI15765NetLayer = this->m_pI15765NetLayer;
    m_threadCanRx->m_pLineEditPCID = this->ui->lineEdit_PCID;
    m_threadCanRx->start();

    m_threadCanTx = new TxThread(this);
    connect(m_threadCanTx, SIGNAL(finished()),this,SLOT(onTxThreadFinished()));
    m_threadCanTx->m_pCanDevice = this->m_pCanDevice;
    m_threadCanTx->m_pI15765NetLayer = this->m_pI15765NetLayer;
    m_threadCanTx->m_pLineEditICID = this->ui->lineEdit_ICNID;
    m_threadCanTx->start();

//    m_threadCanRx->wait();
//    m_threadCanTx->wait();
//    delete m_threadCanRx;
//    delete m_threadCanTx;
}

void Widget::startDebug1()
{
    ;
}

void Widget::onDeviceStatChanged()
{
    if(m_pCanDevice->m_isOpened)
    {
        ui->listWidget->takeItem(0);
        ui->listWidget->addItem(m_pCanDevice->m_sOpenDeviceType);
    }
    else
    {
        ui->listWidget->takeItem(0);
        ui->listWidget->addItem(tr("尚未连接CAN设备"));
    }
}

void Widget::onSetBitRate()
{
    int iBitRateToUse;
    int iNetworkID;
    int iResult;

    // Has the uset open neoVI yet?
    if (!this->m_pCanDevice->m_isOpened)
    {
        QMessageBox::warning(this,tr("提示"),tr("CAN设备尚未打开，打开后重试"));
        return;   // do not read messages if we haven't opened neoVI yet
    }

    //Get the network name index to set the baud rate of
    switch(ui->comboBox_CanChannel->currentIndex())
    {
        case 0:
            iNetworkID = NETID_HSCAN;
            break;
        case 1:
            iNetworkID = NETID_MSCAN;
            break;
        case 2:
            iNetworkID = NETID_SWCAN;
            break;
        case 3:
            iNetworkID = NETID_LSFTCAN;
            break;
        case 4:
            iNetworkID = NETID_FIRE_HSCAN2;
            break;
        case 5:
            iNetworkID = NETID_FIRE_HSCAN3;
            break;
        default:
        QMessageBox::warning(this,tr("Warning"),tr("Incorrect Network selected"));
            return;
    }

    //iBitRateToUse = Convert::ToInt32(lstBaudRateToUse->Text);
    QString str = ui->comboBox_BitRate->currentText();
    iBitRateToUse = str.toInt();

    //Set the bit rate
    iResult = this->m_pCanDevice->setBitRate(iBitRateToUse, iNetworkID);
    if (iResult == 0)
    {
        QMessageBox::warning(this,tr("Warning"),tr("Problem setting bit rate"));
    }
    else
    {
        QMessageBox::warning(this,tr("Warning"),tr("Bit rate Set"));
    }
}

void Widget::onRxThreadFinished()
{
    qDebug() << tr("接收线程完成，开始析构");
    this->m_threadCanTx->m_bStopped = true;
    delete m_threadCanRx;
    ui->pushButton_openFile->setEnabled(true);
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_setBitRate->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_start->setEnabled(true);
    ui->comboBox_BitRate->setEnabled(true);
    ui->comboBox_CanChannel->setEnabled(true);
    ui->lineEdit_ICNID->setEnabled(true);
    ui->lineEdit_PCID->setEnabled(true);
}

void Widget::onTxThreadFinished()
{
    qDebug() << tr("发送收线程完成，开始析构");
    this->m_threadCanRx->m_bStopped = true;
    delete m_threadCanTx;
    ui->pushButton_openFile->setEnabled(true);
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_setBitRate->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(true);
    ui->pushButton_start->setEnabled(true);
    ui->comboBox_BitRate->setEnabled(true);
    ui->comboBox_CanChannel->setEnabled(true);
    ui->lineEdit_ICNID->setEnabled(true);
    ui->lineEdit_PCID->setEnabled(true);
}
