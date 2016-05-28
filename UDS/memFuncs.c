/* 
 * =============================================================================
 * 版权所有 (C), 1966-2012, 成都天兴仪表股份有限公司
 * 文件名称: memFuncs.c
 * 创 建 人：邵龙      日期：2010-12-18 	
 * 文件描述：内存操作函数集                                                  
 * 版    本：V1.0  
 * 修改记录：无                                                   
 * =============================================================================
 */	

#include "uds.h"
#include "stddef.h"

/*板级内存的最大值*/
#define MAX_BOARD_MEM_SIZE				(0x40000)/*256KB*/

/********************************************************************
* 函数名称：memCpyWrapper
* 函数功能：内存拷贝
* 输入参数：bytes: 拷贝长度
					pSrc: 源地址
* 输出参数：pDst: 目的地址
* 返    回：TRUE: 拷贝成功，FALSE: 拷贝失败
* 备    注：无。
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
* 函数名称：memSetWrapper
* 函数功能：内存赋值
* 输入参数：bytes: 内存长度
					pSrc: 需要赋予的值
* 输出参数：pDst: 目的地址
* 返    回：TRUE: 赋值成功，FALSE: 赋值失败
* 备    注：无。
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
* 函数名称：memValidCheck
* 函数功能：检查内存是否非法
* 输入参数：memSize: 内存长度
					memAddr: 内存首地址
* 输出参数：无
* 返    回：TRUE: 可用，FALSE: 非法
* 备    注：无。
********************************************************************/
/*uint8_t API_memValidCheck(uint32_t memAddr, uint16_t memSize)
{
	if((memSize > MAX_BOARD_MEM_SIZE))
	{
	    
		return FALSE;
	}
	else
	{
		//待添加
	}
	return TRUE;
}
*/









