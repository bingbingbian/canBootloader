/******************************************************************************

                  ��Ȩ���� (C), �ɶ������Ǳ�ɷ����޹�˾

 ******************************************************************************
  �� �� ��   : memFuncs.h
  �� �� ��   : ����
  ��    ��   : Tan Zhihua
  ��������   : 2013��1��9��
  ����޸�   :
  ��������   : memFuncs.c ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2013��1��9��
    ��    ��   : Tan Zhihua
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __MEMFUNCS_H__
#define __MEMFUNCS_H__

#include "datatype.h"


extern uint8_t API_memCpy(uint8_t* pDst, const uint8_t* pSrc, uint32_t bytes);
extern uint8_t API_memSet(uint8_t* pDst, uint8_t value, uint32_t bytes);
//extern uint8_t API_memValidCheck(uint32_t memAddr, uint16_t memSize);




#endif /* __MEMFUNCS_H__ */
