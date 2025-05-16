/*********************************************************************************************************
*
* File                : common.h
* Hardware Environment: 
* Build Environment   : RealView MDK-ARM  Version: 4.74
* Version             : V1.0
* Author              : 
* Time                :
* Brief               : 
*********************************************************************************************************/
#ifndef  _COMMON_H
#define  _COMMON_H

#include "type.h"
#include "dbg_log.h"



unsigned short GetCrc16(unsigned short usCrcInit, unsigned char *pData, int iLen);
unsigned int Cal_Crc32(unsigned int init, const unsigned char *data, unsigned int len, unsigned char status);
int ChangeHexStringToHexArray(char *pSrc, unsigned short usLen, char *pDst);
int ChangeHexArrayToHexString(const unsigned char *pszData, int ilen, char * szData);
int ChangeGB2312ToHex(char *pDst, char *pSrc);
unsigned char CheckAndChangeUnicastIP(char * strIP, unsigned char ucLen, unsigned char *ucIP);
unsigned char IPMaskCheck(unsigned char *pMask);
unsigned char CheckIp(unsigned char ucIsMask,unsigned char * ucIP);
unsigned char CheckGateWayIP(unsigned char *pLocalIP, unsigned char *pMask, unsigned char *pLocalGW);
unsigned char CheckAndChangeIP(unsigned char ucIsMask,char * strIP, unsigned char ucLen, unsigned char *ucIP);
unsigned char CheckAndChangeMutiIP(char * strIP, unsigned char ucLen, unsigned char *ucIP);
void urldecode(char *p);
int CheckPassWordRule(char * pwd, unsigned char ucLen);
unsigned int StrToUnsignedint(char *data);

#endif





