/******************************************************************************

                  版权所有 (C), 成都天兴仪表股份有限公司

 ******************************************************************************
  文 件 名   : iso_15765_2_config.h
  版 本 号   : 初稿
  作    者   : Tan Zhihua
  生成日期   : 2012年12月14日
  最近修改   :
  功能描述   : iso 15765-2 协议栈配置信息
  函数列表   :
  修改历史   :
  1.日    期   : 2012年12月14日
    作    者   : Tan Zhihua
    修改内容   : 创建文件

******************************************************************************/

#ifndef _ISO_15765_2_CONFIG_H_
#define _ISO_15765_2_CONFIG_H_

#include "datatype.h"

//#define I15765CFG_SA                    (0x798) //源地址
#define DIAG_BASE_ADRESS                (0x700) //基地址
/*
#define I15765CFG_MF_RX_BUF_NUM         (10)
#define I15765CFG_MF_RX_BUF_SIZE        (100)

#define I15765CFG_MF_TX_BUF_NUM         (3)
#define I15765CFG_MF_TX_BUF_SIZE        (100)

//报文类型
#define SINGLE_FRAME                    (0)     //单帧
#define FIRST_FRAME                     (1)     //首帧
#define CONSECUTIVE_FRAME               (2)     //连续帧
#define FLOW_CONTROL                    (3)     //流控制帧

//FS类型
#define FS_CON_TO_SEND                  (0)     //允许发送方发送
#define FS_WAIT_TO_SEND                 (1)     //等待发送方发送
#define FS_OVER_FLOW                    (2)     //接收方数据超出缓存
*/

//时间等相关参数

/**************************超时时间*****************************/

//CAN经过数据链路层发送时间:<70ms
#define N_AS_MAX_TIME                   (70)

//CAN经过数据链路层接收时间:<70ms
#define N_AR_MAX_TIME                   (70)

//发送方接收流控制帧的等待时间:<150ms
#define N_BS_MAX_TIME                   (150)

//接收方连续帧的等待时间:<150ms
#define N_CR_MAX_TIME                   (150)
/************************性能要求时间***************************/

//接收方发送流控制帧的间:<70ms
#define N_BR_MAX                        (70)    //ms

//发送方连续帧的间:<70ms
#define N_CS_MAX                        (70)    //ms

//两个连续帧之间的最小间隔时间：20ms
#define N_INIT_STMIN                    (20)    //ms

/************************按次定义参数***************************/

//等待流控制帧最大发送次数:0
#define N_WFT_MAX                       (0)

//持续发送次数: 8
#define N_INIT_BS                       (8)

typedef struct
{
    uint8_t mtype;      //信息类型
    uint8_t N_sa;       //网络源地址  (本地 - 服务器 ID)
    uint8_t N_ta;       //网络目标地址  ( 客户机 ID )
    uint8_t N_tatype;   //网络目标地址类型  
    uint8_t ae;         //网络位址扩展    
    uint8_t *DataBUf;   //数据指针 
    uint8_t Length;     //数据长度    
}US_REQ_STRU;

typedef struct
{
    US_REQ_STRU Request;
    
}USDATA_STRU;

typedef struct
{
    uint8_t type    :4;
    uint8_t SF_DL   :4;
    uint8_t DataBuf[7];
}SF_STRU;  //单帧结构 (未用)

typedef struct
{
    uint16_t type   :4;
    uint16_t FF_DL  :12;
    uint8_t DataBuf[6];
}FF_STRU;   //首帧结构 (未用)

typedef struct
{
    uint8_t type    :4;
    uint8_t SN      :4;
    uint8_t DataBuf[7];
}CF_STRU;   //连续帧结构 (未用)

typedef struct
{    
    uint8_t type    :4;
    uint8_t FS      :4;
    uint8_t BS;
    uint8_t STmin;
    uint8_t DataBuf[5];
}FC_STRU;   //流控制帧结构 (未用)

typedef union
{
    uint8_t DataBuf[8];
    SF_STRU SingleFrame;
    FF_STRU FirstFrame;
    CF_STRU ConsecutiveFrame;    
    FC_STRU FlowControlFrame;
}N_PDU_UN;  //网络层帧结构 共用体 ( 未用 )

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

typedef enum
{
    N_CHG_OK,
    N_RX_ON,
    N_WRONG_PARAMETER,
    N_WRONG_VALUE,
}N_IDS_CHG_PARA_ENUM;   //设置参数类型

typedef enum
{
    diagnostics,        //常规诊断报文
    remote_diagnostics  //功能寻址诊断报文
}US_MTYPE_ENUM;        //诊断报文类型

typedef enum
{
    STmin,              // 连续帧最小帧间隔时间
    ESTmin,             // 
    BS,                 // BlockSize 块大小
}N_US_PARAMETER_ENUM;   //网络层参数枚举类型

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

typedef struct 
{
    US_MTYPE_ENUM Mtype;
	uint8_t sa; /* source address */
	uint8_t ta; /* target address */
	uint8_t ae; /* address extension */
	uint8_t tat; /* target address type */
	uint8_t pri; /* priority of message */
    N_US_RESULT_ENUM result;    
} N_UDDATA_CONFIRM_STRU; //确认原语结构体类型  ( 网络层向应用层确认 )

typedef struct 
{
    US_MTYPE_ENUM Mtype;
	uint8_t sa; /* source address */
	uint8_t ta; /* target address */
	uint8_t ae; /* address extension */
	uint8_t tat; /* target address type */
	uint8_t pri; /* priority of message */
    uint8_t length;
} N_UDDATA_FF_IND_STRU; //首帧指示原语结构体类型    (网络层向应用层指示接收到首帧)

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

#endif

