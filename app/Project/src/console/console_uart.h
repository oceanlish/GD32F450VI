#ifndef _CONSOLE_UART_H_
#define _CONSOLE_UART_H_

#include "main.h"
#include "includes.h"

#define USE_UART_CONSOLE

#define DEBUG_UART_LOGIN_SUCCESS   1
#define DEBUG_UART_LOGIN_FAIL      0

#define DEBUG_UART_USR							0
#define DEBUG_UART_PWD							1

#define DEBUG_UART_DOECHO_CMD				0
#define DEBUG_UART_WINSIZE_CMD			1
#define DEBUG_UART_FLOWCTL_CMD			2
#define DEBUG_UART_WILLECHO_CMD			3
#define DEBUG_UART_DOSUPPRESS_CMD		4
#define DEBUG_UART_CMD_END					99

#define MAX_DEBUG_UART_CMD_SIZE				100
#define MAX_DEBUG_UART_CMD_BUFF_LEN				10

#define ESC '\033'

typedef struct 
{
	unsigned char ucState;
	unsigned char ucLogin;
	unsigned char ucUsrIndex;
	char 					szUsr[100];
	unsigned char ucPwdIndex;
	char 					szPwd[100];
}DEBUG_UART_USR_INFO;


typedef struct
{	
	char szCmd[MAX_DEBUG_UART_CMD_SIZE];
}DEBUG_UART_CMD_ITEM;

typedef struct
{
	unsigned char ucRead;
	unsigned char ucWrite;
	unsigned char ucIndex;
	DEBUG_UART_CMD_ITEM stCmdList[MAX_DEBUG_UART_CMD_BUFF_LEN];
}DEBUG_UART_CMD_LINKLIST;

typedef INT8U (*DEBUG_UART_PROC_FUNC)(INT8U ucIndex,char *cmd, char *Param);

typedef struct
{
	char szCmd[MAX_DEBUG_UART_CMD_SIZE];
	INT32U dwParam;
	DEBUG_UART_PROC_FUNC pfnProc;
}DEBUG_UART_CMD_PROC;


int sys_debug_uart_init(void);
void debug_uart_data_send(char* sendbuff,int len);

extern char g_szDebugUartSndBuf[256];
extern unsigned char g_ucDebugUartLogSuspend;

#endif



