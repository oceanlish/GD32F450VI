#ifndef _WIEGAND_H
#define _WIEGAND_H
#include "includes.h"
#include "type.h"
#include "sys_ringbuff.h"


#define MAX_WIEGAND_SIZE			4
#define RINGBUFF_WIEGAND_SIZE		50
typedef struct{
	unsigned char 	len;
	unsigned char		szBuff[MAX_WIEGAND_SIZE];
}ST_Buff_Wiegand;

typedef struct
{
	ST_RingBuff_Header		stHeader;
	ST_Buff_Wiegand		szBuff[RINGBUFF_WIEGAND_SIZE];
}ST_RingBuff_Wiegand;

extern void WiegandBuff_Init(void);
extern int WiegandBuff_Push(void *pUnit);
extern int WiegandBuff_Pop(void *pUnit);
extern int WiegandBuff_GetFreeSize(void);
extern void Wiegand26_Send(INT8U* pData );
extern void Wiegand34_Send(INT8U* pData );
extern int Wiegand26_Recv(INT8U* pData);
#endif



