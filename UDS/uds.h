/* 
 * =============================================================================
 * ��Ȩ���� (C), 1966-2012, �ɶ������Ǳ�ɷ����޹�˾
 * �ļ�����: uds.h
 * �� �� �ˣ�����      ���ڣ�2012-8-12  
 * �ļ���������ϻỰ�ṹ�嶨�塢   ö�ٶ����ԡ����Ͷ����Լ�ȫ�ֱ�������                                                  
 * ��    ����V1.0  
 * �޸ļ�¼����                                                   
 * =============================================================================
 */ 
#ifndef _UDS_H_
#define _UDS_H_

#include "mc9s12hy64.h"
#include "datatype.h"


//#include "i15765cfg.h"
//#include "data_type.h"
#ifndef MAX_UDS_TX_BUF_BYTES
#define MAX_UDS_TX_BUF_BYTES            (200) //���ͺͽ��� Buffer ����ֽ��� ����ʵ��Ӧ�õ��� 
#endif

/*������Ϸ���ID*/
#define UDS_DSC_SID                     (0x10)  //���ģʽ����
#define UDS_ECUR_SID                    (0x11)  //��ص�Ԫ��λ
#define UDS_SA_SID                      (0x27)  //��ȫ����
#define UDS_CC_SID                      (0x28)  //ͨ�ſ���
#define UDS_TP_SID                      (0x3E)  //����豸����
#define UDS_CDTCS_SID                   (0x85)  //����DTC����

#define UDS_RDBI_SID                    (0x22)  //��ȡ����
//#define UDS_RMBA_SID                  (0x23)  //��ȡ�ڴ�
//#define UDS_RDBPI_SID                 (0x2A)  //���ڶ�ȡ����
//#define UDS_WDBI_SID                  (0x2E)  //д������
//#define UDS_WMBA_SID                  (0x3D)  //д���ڴ�

#define UDS_CDI_SID                     (0x14)  //��������Ϣ
#define UDS_RDTCI_SID                   (0x19)  //��ȡDTC��Ϣ

//#define UDS_IOCBI_SID                 (0x2F)  //�����������

//#define UDS_RC_SID                        (0x31)  //���̿���

//#define UDS_RD_SID                        (0x34)  //��������
//#define UDS_RU_SID                        (0x35)  
//#define UDS_TD_SID                        (0x36)  //��������
//#define UDS_RTE_SID                       (0x37)  //�����˳�����

#define UDS_FACTORY_MODE_SID            (0xA0)      //����ģʽ

#define P2_CAN_SERVER_MAX               (50)
#define P2X_CAN_SERVER_MAX              (5000)
#define S3_CAN_SERVER_MAX               (5000)

typedef uint8_t SESSION_STATUS;  
typedef uint8_t SECURITY_STATUS;  
typedef uint8_t I15765_TX_STATUS;  


typedef uint8_t PROG_STATUS;  

typedef uint8_t UDS_ERR_CODE;  

enum    /*��ϻỰ������*/
{
    UDS_NO_ERR = 0,                 /*�޴���*/
    UDS_INPUT_PARAM_ERR,            /*��������Ƿ�*/
    UDS_SESSION_MODE_ERR,           /*�Ựģʽ���󣬲������������Ựģʽ���κ�һ��*/
    UDS_CAN_TX_FAIL,                /*CAN���ߴ���ʧ��*/
    UDS_HARD_RESET_FAIL,            /*Ӳ����λʧ��*/
    UDS_KEY_OFF_ON_RESET_FAIL,      /*���Կ�׸�λʧ��*/
    UDS_SOFT_RESET_FAIL             /*�����λʧ��*/
};


enum    /*�Ựģʽ*/
{
    NULL_SESSION = 0,
    DEFAULT_SESSION,            /*Ĭ�ϻỰģʽ*/
    PROGRAMMING_SESSION,        /*��̻Ựģʽ*/
    EXTENDED_SESSION            /*��չ�Ựģʽ*/
};

enum    /*��ȫģʽ*/
{
    SECURITY_LOCK,              /*����״̬*/
    SECURITY_UNLOCK             /*����״̬*/
};

enum    /*���ģʽ״̬*/
{
    DEFAULT_STATUS = 0,
    PROGRAM_STATUS
};

enum    /*I15765����״̬*/
{
    I15765_SENT,
    I15765_FAILED
};

enum    /*����Ӧ��*/
{
    NRC_SNS         = 0x11,     /*��Ϸ���֧�� */ 
    NRC_SFNS        = 0x12,     /*�ӹ��ܲ�֧��*/
    NRC_IMLOIF      = 0x13,     /*���ĳ��Ȼ��߸�ʽ�Ƿ�*/
    NRC_CNC         = 0x22,     /*����������*/
    NRC_SAD         = 0x33,     /*��ȫ���ʾܾ�*/
    NRC_RSE         = 0x24,     /*����������*/
    NRC_ROOR        = 0x31,     /*������������ݳ�����ֵ��Χ*/
    NRC_IK          = 0x35,     /*��Կ�Ƿ�*/
    NRC_ENOA        = 0x36,     /*������ȫ���ʳ��Դ�������*/
    NRC_RTDNE       = 0x37,     /*��ʱʱ��δ��*/
    NRC_UDNA        = 0x70,     /*����������ĳ�ֹ��Ͼܾ��ϴ�/���ز���*/
    NRC_GPF         = 0x72,     /*д��ʧ��*/
    NRC_RCRRP       = 0x78,     /*��������ȷ���յ��ͻ��˷��͵�����,����δ������*/
    NRC_WBSC        = 0X73,
    NRC_SNSIAS      = 0x7F,     /* �ڵ�ǰģʽ�£���������֧�ֿͻ�������� SID */
    
    /*�Լ����*/
    NRC_RDF         = 0xfe,     /*��ȡʧ��*/
    NRC_OTHER       = 0xff,     /*��������*/
    
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

/*��ǰ�Ựģʽ*/
//extern SESSION_STATUS g_curSessionStatus;

/*��ǰ��ȫ״̬*/
//extern SECURITY_STATUS g_curSecurityStutus;

/*���ģʽ״̬*/
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

extern void i15765_tx_app( i15765_t*msg, I15765_TX_STATUS *txStatus); // i15765Ӧ�ò㷢����Ϣ
extern UDS_ERR_CODE comNckResp(uint8_t reqId, uint8_t nrCode, const can_15765_ctrl_t* pTxCtrl);

#endif



