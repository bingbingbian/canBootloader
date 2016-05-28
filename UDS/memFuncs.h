/******************************************************************************

                  版权所有 (C), 成都天兴仪表股份有限公司

 ******************************************************************************
  文 件 名   : memFuncs.h
  版 本 号   : 初稿
  作    者   : Tan Zhihua
  生成日期   : 2013年1月9日
  最近修改   :
  功能描述   : memFuncs.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年1月9日
    作    者   : Tan Zhihua
    修改内容   : 创建文件

******************************************************************************/

#ifndef __MEMFUNCS_H__
#define __MEMFUNCS_H__

#include "datatype.h"


extern uint8_t API_memCpy(uint8_t* pDst, const uint8_t* pSrc, uint32_t bytes);
extern uint8_t API_memSet(uint8_t* pDst, uint8_t value, uint32_t bytes);
//extern uint8_t API_memValidCheck(uint32_t memAddr, uint16_t memSize);




#endif /* __MEMFUNCS_H__ */
