#ifndef I15765NETLAYER_HXX
#define I15765NETLAYER_HXX

#include <QObject>
#include <QDebug>

#define N_INIT_BS                       (8)     //�������ʹ���
#define N_INIT_STMIN                    (20)   //��������֡����Сʱ����20ms
#define     DATABUFLEN          20
#define     UDS_RX_740          (0)
#define     UDS_RX_7DF          (1)
#define     UDS_RX_MSG_NUM      (2)
#define     UDS_TX_MSG_NUM      (1)
#define     MAX_UDS_TX_BUF_BYTES  (200) //���ͺͽ��� Buffer ����ֽ��� ����ʵ��Ӧ�õ���

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned int uint32_t;

typedef struct {
    uint16_t ID;
    uint8_t dataLen;
    uint8_t dataBuf[DATABUFLEN];
    bool bRxOk;
}UDS_MSG; //�������������������ϱ��Ľṹ

typedef enum
{
    diagnostics,        //������ϱ���
    remote_diagnostics  //����Ѱַ��ϱ���
}US_MTYPE_ENUM;        //��ϱ�������

typedef enum _FS_STATUS
{
    ContinueToSend=0,   /*  �������� */
    Wait,               /*  �ȴ����� */
    OverFlow,           /*  ������� */
}_FS_STATUS_ENUM;   /*������״̬ */

typedef enum
{
    N_OK,
    N_TIMEOUT_A,
    N_TIMEOUT_Bs,
    N_TIMEOUT_Cr,
    N_WRONG_SN,
    N_INVALID_FS,
    N_UNEXP_PDU,
    N_WFT_OVRN,
    N_BUFFER_OVELW,
    N_ERROR
}N_US_RESULT_ENUM;  //������ִ�еĽ��״̬ ö������

typedef struct
{
    US_MTYPE_ENUM Mtype;
    uint8_t sa; /* source address  ������ ID��ȥ����ַ */
    uint8_t ta; /* target address  �ͻ���ID��ȥ����ַ */
    uint8_t ae; /* address extension */
    uint8_t tat; /* target address type */
    uint8_t pri; /* priority of message */
    uint8_t buf[MAX_UDS_TX_BUF_BYTES]; /* pointer to data */
    uint16_t buf_len; /* size of data (in bytes) */
    N_US_RESULT_ENUM result;
} N_UDDATA_IND_STRU;    //ָʾԭ��ṹ������ ( �������Ӧ�ò�ָʾ ���ݽ������ )

typedef struct
{
    US_MTYPE_ENUM Mtype;    //��ϱ�������  ���� or ����Ѱַ
    uint8_t sa; /* source address ������ ID ��ȥ����ַ */
    uint8_t ta; /* target address �ͻ���ID ��ȥ����ַ */
    uint8_t ae; /* address extension  ��չ��ַ  */
    uint8_t tat; /* target address type Ŀ���ַ���� */
    uint8_t pri; /* priority of message  �������ȼ� (Ŀǰδ��) */
    uint8_t buf[MAX_UDS_TX_BUF_BYTES]; /*  �������� pointer to data */
    uint16_t buf_len; /* size of data (in bytes) */
} N_UDDATA_REQ_STRU;    //����ԭ��ṹ������  (Ӧ�ò� ���������)

typedef struct _UDS_CF_STATUS
{
    uint8_t RxOrTxCmpltFlg; //���ͻ�����ճɹ���־
    uint8_t SnCnt;          //֡���
    uint16_t SnAddData;     //����֡����
    uint16_t MsgLthCnt;     //����֡�ѽ��յ���Ϣ���� ��ÿ֡�������ֽ����ۼ�
}_UDS_CF_STATUS_STRU;     //UDS ����֡״̬

class I15765NetLayer : public QObject
{
    Q_OBJECT

public:
    I15765NetLayer(QObject *parent = 0);
    bool isRxBufEmpty();  //�Ƿ������յ���UDS����
    void getUdsMsg(UDS_MSG *pUdsMsg);  //��ȡ�������UDS����
    void dealUdsMsg(); //����ǰ�յ���UDS����
protected:

private:
    bool m_bRxOK;  //��Ӧ�ò�ָʾ���ճɹ�
    UDS_MSG m_UdsMsg;
    N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];  //��ϱ��Ľ���buffer
    N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];  //��ϱ��ķ���buffer
    _UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM];    //��ϱ��Ľ���״̬������
    _UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM];    //��ϱ��ķ���״̬������
    _FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM + UDS_TX_MSG_NUM]; //�����������״̬
    uint8_t API_memCpy(uint8_t* pDst, const uint8_t* pSrc, uint32_t bytes);
    uint8_t API_memSet(uint8_t* pDst, uint8_t value, uint32_t bytes);
    uint8_t Nw_CanMsgRxProc();
    uint8_t NWL_udsRxMsg(UDS_MSG *pMsg);
    uint8_t NWL_SFMsgProc(UDS_MSG *pMsg);
    uint8_t NWL_FFMsgProc(UDS_MSG *pMsg);
    uint8_t NWL_CFMsgProc(UDS_MSG *pMsg);
    uint8_t NWL_FCFMsgProc(UDS_MSG *pMsg);
    uint8_t CheckDealStat();
    void udsclrMsgBufData(uint8_t *Point,uint8_t MsgLength);
    void NWL_CpyData(uint8_t *Dr,uint8_t *Sr,uint8_t MsgLength);
    void initRxMsgData(uint8_t MsgChannel,N_US_RESULT_ENUM result);
    uint8_t NWL_TxFlowControlFrameProc(UDS_MSG *pMsg);

signals:
    void Send_Can_Message(UDS_MSG TxMsg);
};

#endif // I15765NETLAYER_HXX
