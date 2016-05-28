#ifndef I15765NETLAYER_HXX
#define I15765NETLAYER_HXX

#include <QObject>
#include <QDebug>

#define N_INIT_BS                       (8)     //持续发送次数
#define N_INIT_STMIN                    (20)   //两个连续帧间最小时间间隔20ms
#define     DATABUFLEN          20
#define     UDS_RX_740          (0)
#define     UDS_RX_7DF          (1)
#define     UDS_RX_MSG_NUM      (2)
#define     UDS_TX_MSG_NUM      (1)
#define     MAX_UDS_TX_BUF_BYTES  (200) //发送和接收 Buffer 最大字节数 根据实际应用调整

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned int uint32_t;

typedef struct {
    uint16_t ID;
    uint8_t dataLen;
    uint8_t dataBuf[DATABUFLEN];
    bool bRxOk;
}UDS_MSG; //定义输入输出网络层的诊断报文结构

typedef enum
{
    diagnostics,        //常规诊断报文
    remote_diagnostics  //功能寻址诊断报文
}US_MTYPE_ENUM;        //诊断报文类型

typedef enum _FS_STATUS
{
    ContinueToSend=0,   /*  继续发送 */
    Wait,               /*  等待发送 */
    OverFlow,           /*  接收溢出 */
}_FS_STATUS_ENUM;   /*流控制状态 */

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
}N_US_RESULT_ENUM;  //服务项执行的结果状态 枚举类型

typedef struct
{
    US_MTYPE_ENUM Mtype;
    uint8_t sa; /* source address  服务器 ID减去基地址 */
    uint8_t ta; /* target address  客户机ID减去基地址 */
    uint8_t ae; /* address extension */
    uint8_t tat; /* target address type */
    uint8_t pri; /* priority of message */
    uint8_t buf[MAX_UDS_TX_BUF_BYTES]; /* pointer to data */
    uint16_t buf_len; /* size of data (in bytes) */
    N_US_RESULT_ENUM result;
} N_UDDATA_IND_STRU;    //指示原语结构体类型 ( 网络层向应用层指示 数据接收完成 )

typedef struct
{
    US_MTYPE_ENUM Mtype;    //诊断报文类型  常规 or 功能寻址
    uint8_t sa; /* source address 服务器 ID 减去基地址 */
    uint8_t ta; /* target address 客户机ID 减去基地址 */
    uint8_t ae; /* address extension  扩展地址  */
    uint8_t tat; /* target address type 目标地址类型 */
    uint8_t pri; /* priority of message  报文优先级 (目前未用) */
    uint8_t buf[MAX_UDS_TX_BUF_BYTES]; /*  报文数据 pointer to data */
    uint16_t buf_len; /* size of data (in bytes) */
} N_UDDATA_REQ_STRU;    //请求原语结构体类型  (应用层 请求网络层)

typedef struct _UDS_CF_STATUS
{
    uint8_t RxOrTxCmpltFlg; //发送或则接收成功标志
    uint8_t SnCnt;          //帧序号
    uint16_t SnAddData;     //连续帧计数
    uint16_t MsgLthCnt;     //连续帧已接收到消息长度 对每帧的数据字节数累加
}_UDS_CF_STATUS_STRU;     //UDS 连续帧状态

class I15765NetLayer : public QObject
{
    Q_OBJECT

public:
    I15765NetLayer(QObject *parent = 0);
    bool isRxBufEmpty();  //是否处理完收到的UDS报文
    void getUdsMsg(UDS_MSG *pUdsMsg);  //获取待处理的UDS报文
    void dealUdsMsg(); //处理当前收到的UDS报文
protected:

private:
    bool m_bRxOK;  //向应用层指示接收成功
    UDS_MSG m_UdsMsg;
    N_UDDATA_IND_STRU UdsRxMsgBuf[UDS_RX_MSG_NUM];  //诊断报文接收buffer
    N_UDDATA_REQ_STRU UdsTxMsgBuf[UDS_TX_MSG_NUM];  //诊断报文发送buffer
    _UDS_CF_STATUS_STRU UdsRxMsgCnt[UDS_RX_MSG_NUM];    //诊断报文接收状态控制用
    _UDS_CF_STATUS_STRU UdsTxMsgCnt[UDS_TX_MSG_NUM];    //诊断报文发送状态控制用
    _FS_STATUS_ENUM NwlFsStatus[UDS_RX_MSG_NUM + UDS_TX_MSG_NUM]; //网络层流控制状态
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
