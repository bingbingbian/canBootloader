/******************************************************************************

                  ��Ȩ���� (C), �ɶ������Ǳ�ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : iso_15765_2_config.h
  �� �� ��   : ����
  ��    ��   : Tan Zhihua
  ��������   : 2012��12��14��
  ����޸�   :
  ��������   : iso 15765-2 Э��ջ������Ϣ
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��12��14��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef _ISO_15765_2_CONFIG_H_
#define _ISO_15765_2_CONFIG_H_

#include "datatype.h"

//#define I15765CFG_SA                    (0x798) //Դ��ַ
#define DIAG_BASE_ADRESS                (0x700) //����ַ
/*
#define I15765CFG_MF_RX_BUF_NUM         (10)
#define I15765CFG_MF_RX_BUF_SIZE        (100)

#define I15765CFG_MF_TX_BUF_NUM         (3)
#define I15765CFG_MF_TX_BUF_SIZE        (100)

//��������
#define SINGLE_FRAME                    (0)     //��֡
#define FIRST_FRAME                     (1)     //��֡
#define CONSECUTIVE_FRAME               (2)     //����֡
#define FLOW_CONTROL                    (3)     //������֡

//FS����
#define FS_CON_TO_SEND                  (0)     //�����ͷ�����
#define FS_WAIT_TO_SEND                 (1)     //�ȴ����ͷ�����
#define FS_OVER_FLOW                    (2)     //���շ����ݳ�������
*/

//ʱ�����ز���

/**************************��ʱʱ��*****************************/

//CAN����������·�㷢��ʱ��:<70ms
#define N_AS_MAX_TIME                   (70)

//CAN����������·�����ʱ��:<70ms
#define N_AR_MAX_TIME                   (70)

//���ͷ�����������֡�ĵȴ�ʱ��:<150ms
#define N_BS_MAX_TIME                   (150)

//���շ�����֡�ĵȴ�ʱ��:<150ms
#define N_CR_MAX_TIME                   (150)
/************************����Ҫ��ʱ��***************************/

//���շ�����������֡�ļ�:<70ms
#define N_BR_MAX                        (70)    //ms

//���ͷ�����֡�ļ�:<70ms
#define N_CS_MAX                        (70)    //ms

//��������֮֡�����С���ʱ�䣺20ms
#define N_INIT_STMIN                    (20)    //ms

/************************���ζ������***************************/

//�ȴ�������֡����ʹ���:0
#define N_WFT_MAX                       (0)

//�������ʹ���: 8
#define N_INIT_BS                       (8)

typedef struct
{
    uint8_t mtype;      //��Ϣ����
    uint8_t N_sa;       //����Դ��ַ  (���� - ������ ID)
    uint8_t N_ta;       //����Ŀ���ַ  ( �ͻ��� ID )
    uint8_t N_tatype;   //����Ŀ���ַ����  
    uint8_t ae;         //����λַ��չ    
    uint8_t *DataBUf;   //����ָ�� 
    uint8_t Length;     //���ݳ���    
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
}SF_STRU;  //��֡�ṹ (δ��)

typedef struct
{
    uint16_t type   :4;
    uint16_t FF_DL  :12;
    uint8_t DataBuf[6];
}FF_STRU;   //��֡�ṹ (δ��)

typedef struct
{
    uint8_t type    :4;
    uint8_t SN      :4;
    uint8_t DataBuf[7];
}CF_STRU;   //����֡�ṹ (δ��)

typedef struct
{    
    uint8_t type    :4;
    uint8_t FS      :4;
    uint8_t BS;
    uint8_t STmin;
    uint8_t DataBuf[5];
}FC_STRU;   //������֡�ṹ (δ��)

typedef union
{
    uint8_t DataBuf[8];
    SF_STRU SingleFrame;
    FF_STRU FirstFrame;
    CF_STRU ConsecutiveFrame;    
    FC_STRU FlowControlFrame;
}N_PDU_UN;  //�����֡�ṹ ������ ( δ�� )

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

typedef enum
{
    N_CHG_OK,
    N_RX_ON,
    N_WRONG_PARAMETER,
    N_WRONG_VALUE,
}N_IDS_CHG_PARA_ENUM;   //���ò�������

typedef enum
{
    diagnostics,        //������ϱ���
    remote_diagnostics  //����Ѱַ��ϱ���
}US_MTYPE_ENUM;        //��ϱ�������

typedef enum
{
    STmin,              // ����֡��С֡���ʱ��
    ESTmin,             // 
    BS,                 // BlockSize ���С
}N_US_PARAMETER_ENUM;   //��������ö������

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

typedef struct 
{
    US_MTYPE_ENUM Mtype;
	uint8_t sa; /* source address */
	uint8_t ta; /* target address */
	uint8_t ae; /* address extension */
	uint8_t tat; /* target address type */
	uint8_t pri; /* priority of message */
    N_US_RESULT_ENUM result;    
} N_UDDATA_CONFIRM_STRU; //ȷ��ԭ��ṹ������  ( �������Ӧ�ò�ȷ�� )

typedef struct 
{
    US_MTYPE_ENUM Mtype;
	uint8_t sa; /* source address */
	uint8_t ta; /* target address */
	uint8_t ae; /* address extension */
	uint8_t tat; /* target address type */
	uint8_t pri; /* priority of message */
    uint8_t length;
} N_UDDATA_FF_IND_STRU; //��ָ֡ʾԭ��ṹ������    (�������Ӧ�ò�ָʾ���յ���֡)

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

#endif

