#include "dbg_log.h"
#include "console_uart.h"
#include "main.h"
#include "bsp_uart.h"
#include "cli_cmd.h"
#include "sys_timer.h"


void *	debug_uart_fd;
DEBUG_UART_USR_INFO g_stDebugUartUsrInfo;
char g_szDebugUartCmdBuf[MAX_DEBUG_UART_CMD_SIZE] = {0};
unsigned char debug_uart_buff[128] = {0};
unsigned short debug_uart_used_len = 0;
char g_szDebugUartSndBuf[256] = {0};
DEBUG_UART_CMD_LINKLIST g_stDebugUartCmdList;


static INT8U g_ucDebugUartCmdState = DEBUG_UART_DOECHO_CMD;
static INT8U g_ucDebugUartNeedEchoPrompt = 0;
INT8U g_ucDebugUartLogSuspend = 0;
static SysTimerParams g_stLastDebugUartTabRevSysCnt;


static SysTimerParams g_stDebugUartKeepAliveTmr;
static INT8U g_ucDebugUartCmdFlg = 0;
static INT8U g_szDebugUartCmdRevCnt = 0;


char g_szDebugUartDoEchoCmd[3] 		= {0xFF, 0xFD, 0x01};
char g_szDebugUartWinSizeCmd[3] 		= {0xFF, 0xFD, 0x1F};
char g_szDebugUartFlowCtlCmd[3] 		= {0xFF, 0xFD, 0x21};
char g_szDebugUartWillEchoCmd[3] 	= {0xFF, 0xFB, 0x01};
char g_szDebugUartDoSuppressCmd[3]	= {0xFF, 0xFB, 0x03};


unsigned int g_dwDebugUartPingCnt = 0;


const DEBUG_UART_CMD_PROC			g_stDebugUartCmdProc[] = 
{
	{NULL,0,NULL}
};	

void debug_uart_data_send(char* sendbuff,int len)
{
	TransData stTransData;
	
	stTransData.ilen = len;
	stTransData.pbuff = (unsigned char *)sendbuff;
	stTransData.pllif_fd = debug_uart_fd;
	
	g_stGlobeOps.cri_ops.sched_lock();
	g_stGlobeOps.if_ops.write(&stTransData);	
	g_stGlobeOps.cri_ops.sched_unlock();
}

static INT8U Find_Debug_Uart_Prev_Cmd(char * cmd)
{
	//printf("Find_Telnet_Prev_Cmd g_stTelnetCmdList.ucRead %d .ucwrite %d .ucIndex %d\r\n",g_stTelnetCmdList.ucRead,g_stTelnetCmdList.ucWrite,g_stTelnetCmdList.ucIndex);
	//List_Telnet_Cmd_Cache();
	if(g_stDebugUartCmdList.ucRead != g_stDebugUartCmdList.ucWrite)
	{
		if(g_stDebugUartCmdList.ucRead == g_stDebugUartCmdList.ucIndex)
		{
			memcpy(cmd,g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd,strlen(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd));
			return 0xFF;
		}
		if(g_stDebugUartCmdList.ucIndex == 0)
		{
			g_stDebugUartCmdList.ucIndex = MAX_DEBUG_UART_CMD_BUFF_LEN - 1;
		}
		else
		{
			g_stDebugUartCmdList.ucIndex--;
		}

		memcpy(cmd,g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd,strlen(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd));
		return 1;
	}

	return 0;
}


static INT8U Find_Debug_Uart_Next_Cmd(char * cmd)
{
	//printf("Find_Telnet_Next_Cmd g_stTelnetCmdList.ucRead %d .ucwrite %d .ucIndex %d\r\n",g_stTelnetCmdList.ucRead,g_stTelnetCmdList.ucWrite,g_stTelnetCmdList.ucIndex);
	//List_Telnet_Cmd_Cache();
	if(g_stDebugUartCmdList.ucRead != g_stDebugUartCmdList.ucWrite)
	{
		if(((g_stDebugUartCmdList.ucIndex+1)%MAX_DEBUG_UART_CMD_BUFF_LEN == g_stDebugUartCmdList.ucWrite) ||\
			(g_stDebugUartCmdList.ucIndex == g_stDebugUartCmdList.ucWrite))
		{
			memcpy(cmd,g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd,strlen(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd));
			return 0xFF;
		}
		else
		{
			g_stDebugUartCmdList.ucIndex = (g_stDebugUartCmdList.ucIndex+1)%MAX_DEBUG_UART_CMD_BUFF_LEN;
		}

		memcpy(cmd,g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd,strlen(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucIndex].szCmd));
		return 1;
	}

	return 0;
}


static void Debug_Uart_Delete_Remote_Cmdline(INT8U size)
{
	INT8U i = 0;
	INT16U usStrLen = 0;
	
	if(size > 0)
	{
		for(i=0;i<size;i++)
		{				
			sprintf(&g_szDebugUartSndBuf[usStrLen],"\b \b");
			usStrLen += 3;
		}
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
	}
}


/****************************************************************
名  称： void Debug_Uart_Display_Cmd_Prompt(char *cmd)
功  能： 用于显示命令输入提示符及用户已经输入的部分命令
参  数： cmd 已经输入的部分命令如输入部分命令然后双击tab键查找后重新显示
返回值：
*****************************************************************/
void Debug_Uart_Display_Cmd_Prompt(char *cmd)
{
	unsigned short usStrLen = 0;
	SysParams * pstSysParams = NULL;	
	
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	
  if(cmd == NULL)
  {
    usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n[%s@%s %c[1;32m~%c[0;37m]$",g_stDebugUartUsrInfo.szUsr,pstSysParams->ProdectInfo.szProductName,ESC,ESC);
  }
  else
  {
    usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n[%s@%s %c[1;32m~%c[0;37m]$%s",g_stDebugUartUsrInfo.szUsr,pstSysParams->ProdectInfo.szProductName,ESC,ESC,cmd);
  }
	
	debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
}


static void Insert_Debug_Uart_Cmd(char * cmd)
{
	//printf("Insert_Telnet_Cmd %s\r\n",cmd);
	if((strlen(g_stDebugUartCmdList.stCmdList[(g_stDebugUartCmdList.ucWrite+MAX_DEBUG_UART_CMD_BUFF_LEN-1)%MAX_DEBUG_UART_CMD_BUFF_LEN].szCmd) == strlen(cmd))&&\
		(0 == memcmp(g_stDebugUartCmdList.stCmdList[(g_stDebugUartCmdList.ucWrite+MAX_DEBUG_UART_CMD_BUFF_LEN-1)%MAX_DEBUG_UART_CMD_BUFF_LEN].szCmd,cmd,strlen(cmd))))
	{
		g_stDebugUartCmdList.ucIndex = g_stDebugUartCmdList.ucWrite;
		//printf("Insert_Telnet_Cmd Find Same Cmd\r\n");
		return;
	}
	if((g_stDebugUartCmdList.ucWrite+1)%MAX_DEBUG_UART_CMD_BUFF_LEN == g_stDebugUartCmdList.ucRead)
	{
		memset(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucRead].szCmd,0,sizeof(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucRead].szCmd));
		g_stDebugUartCmdList.ucRead = (g_stDebugUartCmdList.ucRead+1)%MAX_DEBUG_UART_CMD_BUFF_LEN;
		
	}

	memcpy(g_stDebugUartCmdList.stCmdList[g_stDebugUartCmdList.ucWrite].szCmd,cmd,strlen(cmd));
	g_stDebugUartCmdList.ucWrite = (g_stDebugUartCmdList.ucWrite+1)%MAX_DEBUG_UART_CMD_BUFF_LEN;
	g_stDebugUartCmdList.ucIndex = g_stDebugUartCmdList.ucWrite;
}


void debug_uart_cmd_process(char *cmd)
{
	int i = 0;
	unsigned char ucFlg = 0;
	unsigned short usStrLen = 0;
	arg_info tmp_arg_info = {0};
	if(cmd[0] == ' ')
	{
		for(i=0;i<strlen(cmd);i++)
		{
			if(cmd[i] != ' ')
			{
				break;
			}
		}
		if(i == strlen(cmd))
		{
			ucFlg = 1;
			if(g_ucDebugUartLogSuspend == 0)
			{
				g_ucDebugUartLogSuspend = 1;
				usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%c[1;33m终端已暂停%c[0;37m",ESC,ESC);						
				debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
			}
			else
			{
				g_ucDebugUartLogSuspend = 0;
				usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%c[1;33m终端已恢复%c[0;37m",ESC,ESC);						
				debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
			}
			return;
		}
	}
	Insert_Debug_Uart_Cmd(cmd);
	if(cmd_parse_info(cmd,&tmp_arg_info) >= 1)
	{	
		if(find_cmd(tmp_arg_info.argv[0],&tmp_arg_info,1) != NULL)
		{
			ucFlg = 1;
		}
	}
	if(0 == ucFlg)
	{	
		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%c[1;31m无效命令: %s%c[0;37m",ESC,g_szDebugUartCmdBuf,ESC);
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);		
	}
}


static int debug_uart_recv(unsigned char *buff,int buff_len)
{
	cmd_tbl_s *start = NULL;
	INT16U i = 0,k = 0;
	INT16U usStrLen = 0;
	INT8U ucRet = 0,ucFind = 0;
	char szCmdTmp[50] = {0};
	char szCmdTmp1[50] = {0};
	unsigned char * pData = NULL;	
	SysParams * pstSysParams = NULL;
	pData = buff;
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	g_stGlobeOps.systimer_ops.get_runtime(&g_stDebugUartKeepAliveTmr);
	if(*pData==0xFF)
	{	
		/*for(i=0;i<q->len;i++)
		{
			printf("0x%x ",*(pData+i));
		}
		printf("\r\n");	
		*/
		switch(g_ucDebugUartCmdState)
		{
			case DEBUG_UART_DOECHO_CMD:
			{
				if(buff_len>=3)
				{	
					g_ucDebugUartCmdState = DEBUG_UART_WINSIZE_CMD;
					debug_uart_data_send(g_szDebugUartWinSizeCmd,3);
				}
			}
			break;
			case DEBUG_UART_WINSIZE_CMD:
			{
				if(buff_len>=3)
				{	
					g_ucDebugUartCmdState = DEBUG_UART_FLOWCTL_CMD;
					debug_uart_data_send(g_szDebugUartFlowCtlCmd,3);
				}
			}
			break;
			case DEBUG_UART_FLOWCTL_CMD:
			{
				if(buff_len>=3)
				{	
					g_ucDebugUartCmdState = DEBUG_UART_WILLECHO_CMD;
					debug_uart_data_send(g_szDebugUartWillEchoCmd,3);
				}
			}
			break;
			case DEBUG_UART_WILLECHO_CMD:
			{
				if(buff_len>=3)
				{	
					g_ucDebugUartCmdState = DEBUG_UART_DOSUPPRESS_CMD;
					debug_uart_data_send(g_szDebugUartDoSuppressCmd,3);
				}
			}
			break;
			case DEBUG_UART_DOSUPPRESS_CMD:
			{
				if(((buff_len)>=3)&&(*(pData+(buff_len)-3)==0xFF)&& (*(pData+(buff_len)-2)==0xFD)&& (*(pData+(buff_len)-1)==0x03))
				{	
					g_ucDebugUartCmdState = DEBUG_UART_CMD_END;
					usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n\r\n           Welcome to %s Application Embedded uC/OS-III Environment           \r\n\r\n",pstSysParams->ProdectInfo.szProductName);
					debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
					
					usStrLen = sprintf(g_szDebugUartSndBuf,"%s login:",pstSysParams->ProdectInfo.szProductName);
					debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);

				}
			}
			break;
			default:
			{
				if(((buff_len)==2)&&(*(pData+1)==0xF1))
				{
					DbgLog(DBG_TELNET_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"Telnet Cmd: No Operation\r\n");
				}
			}
			break;
			
		}
		
		return 0;
	}
	if(buff_len == 3)
	{
		if((*(pData) == 0x1B) && (*(pData+1) == 0x5B))
		{
			switch(*(pData+2))
			{
				case 0x41:
				{
					memset(g_szDebugUartCmdBuf,0,sizeof(g_szDebugUartCmdBuf));
					ucRet = Find_Debug_Uart_Prev_Cmd(g_szDebugUartCmdBuf);
					if(1 == ucRet)
					{
						Debug_Uart_Delete_Remote_Cmdline(g_szDebugUartCmdRevCnt);								
						g_szDebugUartCmdRevCnt = strlen(g_szDebugUartCmdBuf);		
						if((g_ucSerialDbgFlg != DBG_NULL) && (1 == g_ucDebugUartNeedEchoPrompt))
						{
							g_ucDebugUartNeedEchoPrompt =0;
							Debug_Uart_Display_Cmd_Prompt(NULL);	
						}
						debug_uart_data_send(g_szDebugUartCmdBuf,strlen(g_szDebugUartCmdBuf));
					}
					else if(0xFF == ucRet)
					{
						g_szDebugUartCmdRevCnt = strlen(g_szDebugUartCmdBuf);	
					}
				}
				break;
				case 0x42:
				{

					memset(g_szDebugUartCmdBuf,0,sizeof(g_szDebugUartCmdBuf));
					ucRet = Find_Debug_Uart_Next_Cmd(g_szDebugUartCmdBuf);
					if(1 == ucRet)
					{		
						Debug_Uart_Delete_Remote_Cmdline(g_szDebugUartCmdRevCnt);								
						g_szDebugUartCmdRevCnt = strlen(g_szDebugUartCmdBuf);
						if((g_ucSerialDbgFlg != DBG_NULL) && (1 == g_ucDebugUartNeedEchoPrompt))
						{
							g_ucDebugUartNeedEchoPrompt =0;
							Debug_Uart_Display_Cmd_Prompt(NULL);	
						}
						debug_uart_data_send(g_szDebugUartCmdBuf,strlen(g_szDebugUartCmdBuf));
					}
					else if(0xFF == ucRet)
					{
						g_szDebugUartCmdRevCnt = strlen(g_szDebugUartCmdBuf);	
					}
				}
				break;
				case 0x43:
				{
					//right
					//printf("right\r\n");
				}
				break;
				case 0x44:
				{
					//left
					//printf("left\r\n");
				}
				break;
				default:
					break;
			}

			return 0;
		}
	}
	
	for(i=0;i<buff_len;i++)
	{
		if(*(pData+i) == 0x09)
		{
					if(g_stDebugUartUsrInfo.ucLogin == DEBUG_UART_LOGIN_SUCCESS)
					{
						if(g_szDebugUartCmdRevCnt != 0)
						{
							memcpy(szCmdTmp,g_szDebugUartCmdBuf,g_szDebugUartCmdRevCnt);
							//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Need Find %s\r\n",szCmdTmp);
							
							if(g_stGlobeOps.systimer_ops.diff_runtime(&g_stLastDebugUartTabRevSysCnt)<300)
							{
								//---double tab所有符合的指令提示
								//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB Cmd\r\n");
//								for(j=0;j<sizeof(g_stDebugUartCmdProc)/sizeof(DEBUG_UART_CMD_PROC);j++)
//								{									
//									if(memcmp(g_stDebugUartCmdProc[j].szCmd,szCmdTmp,strlen(szCmdTmp)) == 0)
//									{
//										if(0 == ucFind)
//										{
//											usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n");
//											debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
//											ucFind = 1;
//										}										
//										usStrLen = sprintf(g_szDebugUartSndBuf,"%s\r\n",g_stDebugUartCmdProc[j].szCmd);
//										debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
//									}
//								}
//								
//								if(1 == ucFind)
//								{
//									Debug_Uart_Display_Cmd_Prompt(szCmdTmp);	
//								}
//								else
//								{
//									if(g_dwDebugUartPingCnt >1)
//									{
//										//	准备停止Ping操作
//										g_dwDebugUartPingCnt = 1;
//									}
//								}
								
								start =	(cmd_tbl_s *)&cmd_tbl_start;
								while(start < &cmd_tbl_end)
								{
									if(memcmp(start->name,szCmdTmp,strlen(szCmdTmp)) == 0)
									{
										if(0 == ucFind)
										{
											usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n");
											debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
											ucFind = 1;
										}
										usStrLen = sprintf(g_szDebugUartSndBuf,"%s\r\n",start->name);
										debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
									}
									start++;
								}
								if(1 == ucFind)
								{
									Debug_Uart_Display_Cmd_Prompt(szCmdTmp);	
								}
								else
								{
									if(g_dwDebugUartPingCnt >1)
									{
										//	准备停止Ping操作
										g_dwDebugUartPingCnt = 1;
									}
								}
							}
							else
							{
								//---single tab补齐
								//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB Cmd\r\n");
//								for(j=0;j<sizeof(g_stDebugUartCmdProc)/sizeof(DEBUG_UART_CMD_PROC);j++)
//								{									
//									if(memcmp(g_stDebugUartCmdProc[j].szCmd,szCmdTmp,strlen(szCmdTmp)) == 0)
//									{
//										
//										//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"Find <%s>\r\n",g_stTelnetCmdProc[j].szCmd);
//										if(0 == ucFind)
//										{
//											memcpy(szCmdTmp1,g_stDebugUartCmdProc[j].szCmd,strlen(g_stDebugUartCmdProc[j].szCmd));
//											//usStrLen = sprintf(g_szTelnetSndBuf,"\r\n");
//											//tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);										
//										}
//										else
//										{
//											for(k=0;k<strlen(szCmdTmp1);k++)
//											{
//												if(szCmdTmp1[k] == g_stDebugUartCmdProc[j].szCmd[k])
//												{
//													continue;
//												}
//												else
//												{
//													szCmdTmp1[k] = 0;
//													break;
//												}
//											}
//										}
//										ucFind++;								
//									}
//								}

								start =	(cmd_tbl_s *)&cmd_tbl_start;
								while(start < &cmd_tbl_end)
								{
									if(memcmp(start->name,szCmdTmp,strlen(szCmdTmp)) == 0)
									{
										if(0 == ucFind)
										{
											memcpy(szCmdTmp1,start->name,strlen(start->name));
										}
										else
										{
											for(k=0;k<strlen(szCmdTmp1);k++)
											{
												if(szCmdTmp1[k] == start->name[k])
												{
													continue;
												}
												else
												{
													szCmdTmp1[k] = 0;
													break;
												}
											}
										}
										ucFind++;
									}
									start++;
								}
								if(ucFind >= 1)
								{
									//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find --> %s\r\n",szCmdTmp1);
									Debug_Uart_Delete_Remote_Cmdline(g_szDebugUartCmdRevCnt);
									memset(g_szDebugUartCmdBuf,0,sizeof(g_szDebugUartCmdBuf));
									memcpy(g_szDebugUartCmdBuf,szCmdTmp1,strlen(szCmdTmp1));
									g_szDebugUartCmdRevCnt = strlen(szCmdTmp1);
									usStrLen = sprintf(g_szDebugUartSndBuf,"%s",g_szDebugUartCmdBuf);
									debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
								}
								else
								{
									//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Nothing Find...\r\n");
								}
								
								
							}
							g_stGlobeOps.systimer_ops.get_runtime(&g_stLastDebugUartTabRevSysCnt);
						}
						else
						{						
							if(g_stGlobeOps.systimer_ops.diff_runtime(&g_stLastDebugUartTabRevSysCnt)<300)
							{
								//---double tab所有符合的指令提示
								//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB\r\n");
								
								if(g_dwDebugUartPingCnt >1)
								{
									//	准备停止Ping操作
									g_dwDebugUartPingCnt = 1;
								}
							}
							else
							{
								//---single tab补齐
								//DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB\r\n");						
								
							}
							g_stGlobeOps.systimer_ops.get_runtime(&g_stLastDebugUartTabRevSysCnt);
						}
						
					}
					
				}				
				else if((*(pData+i) >= 0x20)&&(*(pData+i) <= 0x7E))
				{
					if(g_stDebugUartUsrInfo.ucLogin == DEBUG_UART_LOGIN_FAIL)
					{
						if(g_stDebugUartUsrInfo.ucState == DEBUG_UART_USR)  //登录名输入/显示
						{
							if(g_stDebugUartUsrInfo.ucUsrIndex < 99)
							{
								g_stDebugUartUsrInfo.szUsr[g_stDebugUartUsrInfo.ucUsrIndex] = *(pData+i);
								g_stDebugUartUsrInfo.ucUsrIndex++;
								//printf("++++++++++++++++++-->%s %u\r\n",g_stDebugUartUsrInfo.szUsr,g_stDebugUartUsrInfo.ucUsrIndex);
								usStrLen = sprintf(g_szDebugUartSndBuf,"%c",*(pData+i));
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
							}
							
						}
						else                                         //密码输入/显示
						{
							if(g_stDebugUartUsrInfo.ucPwdIndex < 99)
							{
								g_stDebugUartUsrInfo.szPwd[g_stDebugUartUsrInfo.ucPwdIndex] = *(pData+i);
								g_stDebugUartUsrInfo.ucPwdIndex++;
								//printf("++++++++++++++++++-->%s\r\n",g_stDebugUartUsrInfo.szPwd);

								usStrLen = sprintf(g_szDebugUartSndBuf,"*");
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
								
							}
						}
					}
					else                                           //指令输入/显示
					{
						if(g_szDebugUartCmdRevCnt < (MAX_DEBUG_UART_CMD_SIZE-1))
						{
							g_szDebugUartCmdBuf[g_szDebugUartCmdRevCnt] = *(pData+i);
							g_szDebugUartCmdRevCnt++;
							usStrLen = sprintf(g_szDebugUartSndBuf,"%c",*(pData+i));
							debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
						}
					}
					
				}
				else if(*(pData+i) == 0x08)
				{
					// delete
					if(g_stDebugUartUsrInfo.ucLogin == DEBUG_UART_LOGIN_FAIL)
					{
						if(g_stDebugUartUsrInfo.ucState == DEBUG_UART_USR)  //登录名删除/显示
						{
							if(g_stDebugUartUsrInfo.ucUsrIndex > 0)
							{
								g_stDebugUartUsrInfo.ucUsrIndex--;
								g_stDebugUartUsrInfo.szUsr[g_stDebugUartUsrInfo.ucUsrIndex] = 0;
								//printf("------------>%s\r\n",g_stTelnetUsrInfo.szUsr);
								usStrLen = sprintf(g_szDebugUartSndBuf,"\b \b");
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
								
							}
							else
							{
								
							}
							
						}
						else                                         //密码删除/显示
						{
							if(g_stDebugUartUsrInfo.ucPwdIndex > 0)
							{
								g_stDebugUartUsrInfo.ucPwdIndex--;
								g_stDebugUartUsrInfo.szPwd[g_stDebugUartUsrInfo.ucPwdIndex] = 0;
								//printf("------------>%s\r\n",g_stTelnetUsrInfo.szPwd);
								usStrLen = sprintf(g_szDebugUartSndBuf,"\b \b");
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
							}
							else
							{
									
							}
						}
					}
					else                                           //指令删除/显示
					{
						if(g_szDebugUartCmdRevCnt > 0)
						{
							g_szDebugUartCmdRevCnt--;
							g_szDebugUartCmdBuf[g_szDebugUartCmdRevCnt] = 0;
							g_stDebugUartCmdList.ucIndex = g_stDebugUartCmdList.ucWrite;
							usStrLen = sprintf(g_szDebugUartSndBuf,"\b \b");
							debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);							
						}
					}				
				
				}
				else if(*(pData+i) == 0x0D)
				{
					g_ucDebugUartCmdFlg = 0x0D;
					//---Cmd Enter End	
					if(g_stDebugUartUsrInfo.ucLogin == DEBUG_UART_LOGIN_FAIL)
					{
						//---登录名输入完成
						if(g_stDebugUartUsrInfo.ucState == DEBUG_UART_USR)
						{
							if(g_stDebugUartUsrInfo.ucUsrIndex == 0)
							{
								usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%s login:",pstSysParams->ProdectInfo.szProductName);
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
							}
							else
							{
								usStrLen = sprintf(g_szDebugUartSndBuf,"\r\npassword:");
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
								g_stDebugUartUsrInfo.ucState = DEBUG_UART_PWD;				
							}
							//printf("ucstate %u\n",g_stDebugUartUsrInfo.ucState);		
						}
						//---密码输入完成
						else
						{
							if(g_stDebugUartUsrInfo.ucPwdIndex == 0)
							{
								usStrLen = sprintf(g_szDebugUartSndBuf,"\r\npassword:");
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
							}
							else
							{								
								//---登录成功

								if((strlen(g_stDebugUartUsrInfo.szUsr) == 4) &&(strlen(g_stDebugUartUsrInfo.szPwd) == 9) &&\
									(strcmp(g_stDebugUartUsrInfo.szUsr,"root")==0) && (strcmp(g_stDebugUartUsrInfo.szPwd,"Hik12345+")==0)) 
								{
									g_stDebugUartUsrInfo.ucLogin = DEBUG_UART_LOGIN_SUCCESS;
									g_stGlobeOps.systimer_ops.get_runtime(&g_stDebugUartKeepAliveTmr);
									Debug_Uart_Display_Cmd_Prompt(NULL);		
								}
								//---登录失败
								else
								{
									usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%c[1;31m name or password error! Please try again!%c[0;37m",ESC,ESC);
									debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
									
									usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%s login:",pstSysParams->ProdectInfo.szProductName);
									debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
									memset(&g_stDebugUartUsrInfo,0,sizeof(g_stDebugUartUsrInfo));
								}
							}
							
						}

						
					}
					//---指令输入完成
					else
					{
						g_szDebugUartCmdBuf[g_szDebugUartCmdRevCnt] = 0;
						g_szDebugUartCmdRevCnt = 0;
						if(strlen(g_szDebugUartCmdBuf) > 0)
						{
							//DbgLog(DBG_TELNET_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"Telnet Cmd: [%s]\r\n",g_szTelnetCmdBuf);
							if(0 == strcmp(g_szDebugUartCmdBuf,"quit"))
							{	
								g_dwSerialDbgType = DBG_NULL;
								g_ucSerialDbgFlg = DBG_NULL;	
								g_stDebugUartUsrInfo.ucState = DEBUG_UART_USR;
								g_stDebugUartUsrInfo.ucLogin = DEBUG_UART_LOGIN_FAIL;
								g_stDebugUartUsrInfo.ucUsrIndex = 0;								
								
								usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%s login:",pstSysParams->ProdectInfo.szProductName);
								debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
								memset(&g_stDebugUartUsrInfo,0,sizeof(g_stDebugUartUsrInfo));	
								return 0;
							}
							else
							{
								debug_uart_cmd_process(g_szDebugUartCmdBuf);											
							}
						}
						
						g_ucDebugUartNeedEchoPrompt = 0;
						Debug_Uart_Display_Cmd_Prompt(NULL);	
					}
				}
				else if((*(pData+i) == 0x0A) && (g_ucDebugUartCmdFlg == 0x0D))
				{
					g_ucDebugUartCmdFlg = 0x00;
					
				}
				else if(*(pData+i) == 0x03)
				{
					DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Shut Down Telnet TCP Connection! Because Client Send 0x03 Cmd!\r\n");
					return -1;
				}
			}
		return 0;
}
				
	


int sys_debug_uart_data_handle(void)
{
	RevData stRevData = {0};
	unsigned short usStrLen = 0;
	SysParams * pstSysParams = NULL;	
	
	g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
	
	while(1)
	{
		stRevData.pllif_fd					= debug_uart_fd;
		stRevData.want_len					= sizeof(debug_uart_buff) - debug_uart_used_len;
		stRevData.pbuff							= &debug_uart_buff[debug_uart_used_len];
		
		g_stGlobeOps.if_ops.read(&stRevData);
		debug_uart_used_len += stRevData.actual_len;
		
		if(stRevData.actual_len > 0)
		{
			debug_uart_recv(debug_uart_buff,debug_uart_used_len);
			debug_uart_used_len = 0;	
			if(debug_uart_used_len > 128)
			{
				debug_uart_used_len = 0;
			}
			break;
		}	
		else
		{
			break;
		}
		
	}
	

	if((g_stDebugUartUsrInfo.ucLogin != DEBUG_UART_LOGIN_FAIL)&&(g_stGlobeOps.systimer_ops.diff_runtime(&g_stDebugUartKeepAliveTmr)>600*1000))
	{
		g_dwSerialDbgType = DBG_NULL;
		g_ucSerialDbgFlg = DBG_NULL;	
		g_stDebugUartUsrInfo.ucState = DEBUG_UART_USR;
		g_stDebugUartUsrInfo.ucLogin = DEBUG_UART_LOGIN_FAIL;
		g_stDebugUartUsrInfo.ucUsrIndex = 0;	

		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%c[1;31mlogin timeout! Please try again!%c[0;37m",ESC,ESC);
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
		
		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%s login:",pstSysParams->ProdectInfo.szProductName);
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
		memset(&g_stDebugUartUsrInfo,0,sizeof(g_stDebugUartUsrInfo));		
	}
	
	return 0;

}PER_REGISTER(sys_debug_uart_data_handle);




int sys_debug_uart_init(void)
{
	UartInitParams	stUartInitParams = {0};
	llifParams		params;	
	stUartInitParams.index	= COM0;  
	stUartInitParams.baud = 115200;
	stUartInitParams.dmaEn = 1;
	stUartInitParams.console = 1;
	
	params.inf_type	= INF_UART;
	params.params		= &stUartInitParams;


	g_stGlobeOps.systimer_ops.get_runtime(&g_stLastDebugUartTabRevSysCnt);
	g_stGlobeOps.systimer_ops.get_runtime(&g_stDebugUartKeepAliveTmr);
	
	if( -1 == g_stGlobeOps.if_ops.open(&params,&debug_uart_fd))
	{
		logs(DBG_TELNET_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"[Console Uart] Uart %u Open error!\n",stUartInitParams.index);
		return -1;
	}
	
	return 0;
}MODULE_INIT(sys_debug_uart_init);





