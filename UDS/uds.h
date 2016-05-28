/* 
 * =============================================================================
 * 版权所有 (C), 1966-2012, 成都天兴仪表股份有限公司
 * 文件名称: uds.h
 * 创 建 人：邵龙      日期：2012-8-12  
 * 文件描述：诊断会话结构体定义、   枚举定义以、类型定义以及全局变量声明                                                  
 * 版    本：V1.0  
 * 修改记录：无                                                   
 * =============================================================================
 */ 
#ifndef _UDS_H_
#define _UDS_H_

#include "mc9s12hy64.h"
#include "datatype.h"


//#include "i15765cfg.h"
//#include "data_type.h"
#ifndef MAX_UDS_TX_BUF_BYTES
#define MAX_UDS_TX_BUF_BYTES            (200) //发送和接收 Buffer 最大字节数 根据实际应用调整 
#endif

/*定义诊断服务ID*/
#define UDS_DSC_SID                     (0x10)  //诊断模式控制
#define UDS_ECUR_SID                    (0x11)  //电控单元复位
#define UDS_SA_SID                      (0x27)  //安全访问
#define UDS_CC_SID                      (0x28)  //通信控制
#define UDS_TP_SID                      (0x3E)  //诊断设备在线
#define UDS_CDTCS_SID                   (0x85)  //控制DTC设置

#define UDS_RDBI_SID                    (0x22)  //读取数据
//#define UDS_RMBA_SID                  (0x23)  //读取内存
//#define UDS_RDBPI_SID                 (0x2A)  //周期读取数据
//#define UDS_WDBI_SID                  (0x2E)  //写入数据
//#define UDS_WMBA_SID                  (0x3D)  //写入内存

#define UDS_CDI_SID                     (0x14)  //清除诊断信息
#define UDS_RDTCI_SID                   (0x19)  //读取DTC信息

//#define UDS_IOCBI_SID                 (0x2F)  //输入输出控制

//#define UDS_RC_SID                        (0x31)  //例程控制

//#define UDS_RD_SID                        (0x34)  //请求下载
//#define UDS_RU_SID                        (0x35)  
//#define UDS_TD_SID                        (0x36)  //发送数据
//#define UDS_RTE_SID                       (0x37)  //请求退出发送

#define UDS_FACTORY_MODE_SID            (0xA0)      //工厂模式

#define P2_CAN_SERVER_MAX               (50)
#define P2X_CAN_SERVER_MAX              (5000)
#define S3_CAN_SERVER_MAX               (5000)

typedef uint8_t SESSION_STATUS;  
typedef uint8_t SECURITY_STATUS;  
typedef uint8_t I15765_TX_STATUS;  


typedef uint8_t PROG_STATUS;  

typedef uint8_t UDS_ERR_CODE;  

enum    /*诊断会话错误定义*/
{
    UDS_NO_ERR = 0,                 /*无错误*/
    UDS_INPUT_PARAM_ERR,            /*输入参数非法*/
    UDS_SESSION_MODE_ERR,           /*会话模式错误，不在属于三个会话模式中任何一个*/
    UDS_CAN_TX_FAIL,                /*CAN总线传输失败*/
    UDS_HARD_RESET_FAIL,            /*硬件复位失败*/
    UDS_KEY_OFF_ON_RESET_FAIL,      /*点火钥匙复位失败*/
    UDS_SOFT_RESET_FAIL             /*软件复位失败*/
};


enum    /*会话模式*/
{
    NULL_SESSION = 0,
    DEFAULT_SESSION,            /*默认会话模式*/
    PROGRAMMING_SESSION,        /*编程会话模式*/
    EXTENDED_SESSION            /*扩展会话模式*/
};

enum    /*安全模式*/
{
    SECURITY_LOCK,              /*锁定状态*/
    SECURITY_UNLOCK             /*解锁状态*/
};

enum    /*编程模式状态*/
{
    DEFAULT_STATUS = 0,
    PROGRAM_STATUS
};

enum    /*I15765发送状态*/
{
    I15765_SENT,
    I15765_FAILED
};

enum    /*否定响应码*/
{
    NRC_SNS         = 0x11,     /*诊断服务不支持 */ 
    NRC_SFNS        = 0x12,     /*子功能不支持*/
    NRC_IMLOIF      = 0x13,     /*报文长度或者格式非法*/
    NRC_CNC         = 0x22,     /*条件不满足*/
    NRC_SAD         = 0x33,     /*安全访问拒绝*/
    NRC_RSE         = 0x24,     /*请求次序错误*/
    NRC_ROOR        = 0x31,     /*请求的数据内容超出数值范围*/
    NRC_IK          = 0x35,     /*密钥非法*/
    NRC_ENOA        = 0x36,     /*超出安全访问尝试次数限制*/
    NRC_RTDNE       = 0x37,     /*延时时间未到*/
    NRC_UDNA        = 0x70,     /*服务器由于某种故障拒绝上传/下载操作*/
    NRC_GPF         = 0x72,     /*写入失败*/
    NRC_RCRRP       = 0x78,     /*服务器正确接收到客户端发送的请求,但尚未处理完*/
    NRC_WBSC        = 0X73,
    NRC_SNSIAS      = 0x7F,     /* 在当前模式下，服务器不支持客户端请求的 SID */
    
    /*自己添加*/
    NRC_RDF         = 0xfe,     /*读取失败*/
    NRC_OTHER       = 0xff,     /*其它错误*/
    
};

typedef struct 
{
    uint8_t sa; /* source address */
    uint8_t ta; /* target address */
    uint8_t ae; /* address extension */
    uint8_t tat; /* target address type */
    uint8_t pri; /* priority of message */
    uint8_t *buf; /* pointer to data */
    uint16_t buf_len; /* size of data (in bytes) */
} i15765_t;

typedef struct 
{
    uint8_t sa; /* source address */
    uint8_t ta; /* target address */
    uint8_t ae; /* address extension */
    uint8_t tat; /* target address type */
    uint8_t pri; /* priority of message */
} can_15765_ctrl_t;

/*当前会话模式*/
//extern SESSION_STATUS g_curSessionStatus;

/*当前安全状态*/
//extern SECURITY_STATUS g_curSecurityStutus;

/*编程模式状态*/
//extern PROG_STATUS g_ProgStatus;

#ifndef _FLG_
#define _FLG_

typedef struct 
{
     uint8_t bit7:1;
     uint8_t bit6:1;
     uint8_t bit5:1;
     uint8_t bit4:1;
     uint8_t bit3:1;
     uint8_t bit2:1;
     uint8_t bit1:1;
     uint8_t bit0:1;
}FLG_STRU;

typedef union
{
    uint8_t stats;
    FLG_STRU flg;
}_FLG_UN;

#endif

extern void i15765_tx_app( i15765_t*msg, I15765_TX_STATUS *txStatus); // i15765应用层发送消息
extern UDS_ERR_CODE comNckResp(uint8_t reqId, uint8_t nrCode, const can_15765_ctrl_t* pTxCtrl);

#endif



