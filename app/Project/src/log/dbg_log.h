#ifndef  __DBG_LOG_H__
#define  __DBG_LOG_H__
#include "type.h"
#define DBG_NULL			0 
#define DBG_ERR				1 
#define DBG_INFO			2 
#define DBG_DETAIL		3 
#define DBG_MSG				4 
#define DBG_RAW				5 

#define DBG_RAW_EN

#define DBG_TS_EN			1
#define DBG_TS_DIS		0

#define DBG_COLOR_NULL			0
#define DBG_COLOR_RED				1 
#define DBG_COLOR_YELLOW		2 
#define DBG_COLOR_GREEN			3 


#define DBG_RTC_SW						0x00000001
#define DBG_CPU_USAGE					0x00000002
#define DBG_TCP_SW						0x00000004
#define DBG_TELNET_SW					0x00000008
#define DBG_UDP_SW						0x00000010
#define DBG_HTTP_SW						0x00000020
#define DBG_TCPCLIENT_SW			0x00000040
#define DBG_INTERFACE_SW			0x00000080
#define DBG_TASK_SW					0x00000100
#define DBG_INTERFLASH_SW			0x00000200
#define DBG_SPIFlASH_SW				0x00000400
#define DBG_UART_SW						0x00000800
#define DBG_SYS_SW						0x00001000
#define DBG_GPIO_SW						0x00002000
#define DBG_PROT_SW						0x00004000
#define DBG_RINGBUFF_SW				0x00008000
#define DBG_CAN_SW						0x00010000
#define DBG_CANDM_SW					0x00020000
#define DBG_RECORD_SW					0x00040000
#define DBG_TCPSERVER_SW			0x00080000
#define DBG_USER_SW						0x00100000
#define DBG_AT_SW							0x00200000
#define DBG_LTE_SW						0x00400000
#define DBG_RFID_MODULE_SW				0x00800000
#define DBG_RFID_APP_SW					0x01000000
#define DBG_WIEGAND_SW					0x02000000

#define DBG_APP_SW						0x40000000
#define DBG_ALL_ERR_SW				0x80000000

#define DBG_ALL								0xFFFFFFFF


#define DEBUG

#ifdef DEBUG
#define logs(dwType,ucLevel,ucColor,ucTSEnable,fmt, ...)  DbgLog(dwType,ucLevel,ucColor,ucTSEnable,fmt, ##__VA_ARGS__)
#else
#define logs(dwType,ucLevel,ucColor,ucTSEnable,fmt, ...) 
#endif
void DbgLogInit(void);
void DbgLog(INT32U dwType,INT8U ucLevel,INT8U ucColor,INT8U ucTSEnable,char *fmt, ...);
void DbgLogSendPeriodHandler(void);
int DbgLogGetLeftMsgLen(void);
extern unsigned char g_NeedDbgLevel;
extern INT32U				g_dwNetDbgType,g_dwSerialDbgType;
extern unsigned int	g_dwSerialDbgType;
extern unsigned char g_ucSerialDbgFlg;
#endif


