/* 
 * =============================================================================
 * ��Ȩ���� (C), 1966-2012, �ɶ������Ǳ�ɷ����޹�˾
 * �ļ�����: memFuncs.c
 * �� �� �ˣ�����      ���ڣ�2010-12-18 	
 * �ļ��������ڴ����������                                                  
 * ��    ����V1.0  
 * �޸ļ�¼����                                                   
 * =============================================================================
 */	

#include "uds.h"
#include "stddef.h"

/*�弶�ڴ�����ֵ*/
#define MAX_BOARD_MEM_SIZE				(0x40000)/*256KB*/

/********************************************************************
* �������ƣ�memCpyWrapper
* �������ܣ��ڴ濽��
* ���������bytes: ��������
					pSrc: Դ��ַ
* ���������pDst: Ŀ�ĵ�ַ
* ��    �أ�TRUE: �����ɹ���FALSE: ����ʧ��
* ��    ע���ޡ�
********************************************************************/
uint8_t API_memCpy(uint8_t* pDst, const uint8_t* pSrc, uint32_t bytes)
{
	uint32_t i;
	if((pDst == NULL) || (pSrc == NULL) || (bytes > MAX_BOARD_MEM_SIZE))
	{
		return FALSE;
	}
	else
	{
		for(i=0; i<bytes; i++)
		{
			*pDst++ = *pSrc++;
		}
	}
	return TRUE;
}

/********************************************************************
* �������ƣ�memSetWrapper
* �������ܣ��ڴ渳ֵ
* ���������bytes: �ڴ泤��
					pSrc: ��Ҫ�����ֵ
* ���������pDst: Ŀ�ĵ�ַ
* ��    �أ�TRUE: ��ֵ�ɹ���FALSE: ��ֵʧ��
* ��    ע���ޡ�
********************************************************************/
uint8_t API_memSet(uint8_t* pDst, uint8_t value, uint32_t bytes)
{
	uint32_t i;
	if((pDst == NULL) || (bytes > MAX_BOARD_MEM_SIZE))
	{
		return FALSE;
	}
	else
	{
		for(i=0; i<bytes; i++)
		{
			*pDst++ = value;
		}
	}
	return TRUE;
}


/********************************************************************
* �������ƣ�memValidCheck
* �������ܣ�����ڴ��Ƿ�Ƿ�
* ���������memSize: �ڴ泤��
					memAddr: �ڴ��׵�ַ
* �����������
* ��    �أ�TRUE: ���ã�FALSE: �Ƿ�
* ��    ע���ޡ�
********************************************************************/
/*uint8_t API_memValidCheck(uint32_t memAddr, uint16_t memSize)
{
	if((memSize > MAX_BOARD_MEM_SIZE))
	{
	    
		return FALSE;
	}
	else
	{
		//�����
	}
	return TRUE;
}
*/









