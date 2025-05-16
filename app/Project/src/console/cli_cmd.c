#include "dbg_log.h"
#include "cli_cmd.h"
#include "console_uart.h"
#include "main.h"

cmd_tbl_s cmd_tbl_start	__attribute__ ((used,section ("debug_cmd.item."DEBUG_CMD_START))) = { NULL };
cmd_tbl_s cmd_tbl_end		__attribute__ ((used,section ("debug_cmd.item."DEBUG_CMD_END)))		= { NULL };


/* find command table entry for a command */
struct cmd_tbl *find_cmd_tbl(const char *cmd, struct cmd_tbl *table,
			     int table_len)
{

	struct cmd_tbl *cmdtp;
	struct cmd_tbl *cmdtp_temp = table;	/* Init value */
	const char *p;
	int len;
	int n_found = 0;

	if (!cmd)
		return NULL;
	/*
	 * Some commands allow length modifiers (like "cp.b");
	 * compare command name only until first dot.
	 */
	len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);

	for (cmdtp = table; cmdtp != table + table_len; cmdtp++) {
		if (strncmp(cmd, cmdtp->name, len) == 0) {
			if (len == strlen(cmdtp->name))
				return cmdtp;	/* full match */

			cmdtp_temp = cmdtp;	/* abbreviated command ? */
			n_found++;
		}
	}
	if (n_found == 1) {			/* exactly one match */
		return cmdtp_temp;
	}


	return NULL;	/* not found or ambiguous command */
}


static int cmd_call(cmd_tbl_s *cmdtp, int flag, int argc,
		    char *const argv[])
{
	int result;
	unsigned short usStrLen = 0;
	result = cmdtp->cmd(cmdtp, flag, argc, argv);
	if (result)
	{
		if(flag == 0)
		{
			usStrLen = sprintf(g_szTelnetSndBuf,"\r\nCommand failed, result=%d\r\n",result);	
			tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
		}
		else if(flag == 1)
		{
			usStrLen = sprintf(g_szDebugUartSndBuf,"\r\nCommand failed, result=%d\r\n",result);	
			debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
		}
			
	}
	return result;
}


int cmd_usage(const struct cmd_tbl *cmdtp,int flags)
{
	unsigned short usStrLen = 0;
	
	if(flags == 0)
	{
		usStrLen = sprintf(g_szTelnetSndBuf,"\r\n%s - %s\r\n",cmdtp->name,cmdtp->usage);	
		tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
			

		
		
	
		if (!cmdtp->help) {
			usStrLen = sprintf(g_szTelnetSndBuf,"- No additional help available.\r\n");	
			tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
			return 1;
		}
		
		usStrLen = sprintf(g_szTelnetSndBuf,"\r\nUsage:\n%s\r\n",cmdtp->name);	
		tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
		
		usStrLen = sprintf(g_szTelnetSndBuf,"%s\r\n",cmdtp->help);	
		tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

	}
	else if(flags == 1)
	{
		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\n%s - %s\r\n",cmdtp->name,cmdtp->usage);	
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
			

		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\nUsage:\n%s\r\n",cmdtp->name);	
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
	
		if (!cmdtp->help) {
			usStrLen = sprintf(g_szDebugUartSndBuf,"- No additional help available.\r\n");	
			debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);
			return 1;
		}

		usStrLen = sprintf(g_szDebugUartSndBuf,"%s\r\n",cmdtp->help);	
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	

	}
	return 0;
}


cmd_tbl_s *find_cmd(const char *cmd,arg_info *info,int flag)
{
	int len = 0;
	const char *p;
	cmd_tbl_s *start = NULL;
	start =	(cmd_tbl_s *)&cmd_tbl_start;

	if (!cmd)
		return NULL;
	while(start < &cmd_tbl_end)
	{
		len = ((p = strchr(cmd, '.')) == NULL) ? strlen (cmd) : (p - cmd);
		if (strncmp(cmd, start->name, len) == 0)
		{
			if (len == strlen(start->name))
			{
				if(info->argc <= start->maxargs)
				{
					cmd_call(start,flag,info->argc,info->argv);
					return start;	/* full match */
				}
				else
				{
					cmd_usage(start,flag);
					return NULL;
				}
			}
				
		}
		start++;
	}
	return NULL;
}






int cmd_parse_info(char *line,arg_info *info)
{
	memset(info,0,sizeof(arg_info));
	if(line == NULL)
	{
		return -1;
	}
	int nargs = 0;
	
	while (nargs < MAX_ARGC_COUNT) {
		/* skip any white space */
		while (isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			info->argv[nargs] = NULL;
			printf("%s: nargs=%d\n", __func__, nargs);
			info->argc = nargs;
			return nargs;
		}

		info->argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && !isblank(*line))
			++line;

		if (*line == '\0') {	/* end of line, no more args	*/
			info->argv[nargs] = NULL;
			//printf("parse_line: nargs=%d\n", nargs);
			info->argc = nargs;
			return nargs;
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	printf("** Too many args (max. %d) **\n", 10);

	printf("%s: nargs=%d\n", __func__, nargs);
	return nargs;
}

typedef struct{
	char *debug_flag_type;
	int  debug_flag;
}Debug_Cmd;

const Debug_Cmd debug_type_cmd[] = {
	{"rtc"			,	DBG_RTC_SW},
	{"cpu"			,	DBG_CPU_USAGE},
	{"tcp"			,	DBG_TCP_SW},
	{"telnet"		,	DBG_TELNET_SW},
	{"udp"			,   DBG_UDP_SW},
	{"http"			,	DBG_HTTP_SW},
	{"interface"	,	DBG_INTERFACE_SW},
	{"task"			,   DBG_TASK_SW},
	{"interflash"	,   DBG_INTERFLASH_SW},
	{"nand"			,   DBG_SPIFlASH_SW},
	{"uart"			,   DBG_UART_SW},	
	{"sys"			,   DBG_SYS_SW}, 
	{"gpio"			,   DBG_GPIO_SW},	
	{"protocol"		,	DBG_PROT_SW},
	{"ringbuff"		,   DBG_RINGBUFF_SW},
	{"can"			,   DBG_CAN_SW},
	{"candm"		,   DBG_CANDM_SW},
	{"record"		,   DBG_RECORD_SW},
	{"tcp_server", DBG_TCPSERVER_SW},	
	{"user" 		,		DBG_USER_SW},						
	{"at"			,			 DBG_AT_SW},							
	{"lte" 			,	DBG_LTE_SW},						
	{"module"		,	DBG_RFID_MODULE_SW},		
	{"app"			,	DBG_RFID_APP_SW},
	{"wiegand"		,	DBG_WIEGAND_SW},
	{"allerror"		,   DBG_ALL_ERR_SW}, 
	{"all"			,   DBG_ALL},
	{"level"		,	0x5a55},
	{NULL			,	DBG_ALL}
};

typedef struct{
	char *debug_level_type;
	char debug_level;
}Debug_Level;

const Debug_Level debug_level_cmd[] = {
	{"-r"	,	DBG_RAW},
	{"-m"	,	DBG_MSG},
	{"-d"	,	DBG_DETAIL},
	{"-i"	,	DBG_INFO},
	{"-e"	,	DBG_ERR},
	{"-n"	,	DBG_NULL},
	{NULL	,	DBG_NULL}
};



int debug_process(struct cmd_tbl *cmd, int flags, int argc,char *const argv[])
{
	uint8_t i = 0,cmp_flag = 0;
	INT32U	dwNetDbgTypeTmp = 0;
	if(argc < 2)
	{
		cmd_usage(cmd,flags);
		return -1;
	}
	else if(argc == 2)
	{
		while(debug_type_cmd[i].debug_flag_type != NULL)
		{
			if((strncmp(argv[1],debug_type_cmd[i].debug_flag_type,strlen(debug_type_cmd[i].debug_flag_type)) == 0) \
				&& (strlen(argv[1]) == strlen(debug_type_cmd[i].debug_flag_type)))
			{
				if(debug_type_cmd[i].debug_flag != 0x5a55)
				{
					if(flags == 0)
					{
						g_dwNetDbgType |= debug_type_cmd[i].debug_flag;
					}
					else if(flags == 1)
					{
						g_dwSerialDbgType |= debug_type_cmd[i].debug_flag;
					}
					return 0;
				}
				else
				{
					cmd_usage(cmd,flags);
					return -1;
				}
			}
			i++;
		}
		cmd_usage(cmd,flags);
		return -1;
	}
	else if(argc == 3)
	{
		i = 0;
		while(debug_type_cmd[i].debug_flag_type != NULL)
		{
			if((strncmp(argv[1],debug_type_cmd[i].debug_flag_type,strlen(debug_type_cmd[i].debug_flag_type)) == 0) \
				&& (strlen(argv[1]) == strlen(debug_type_cmd[i].debug_flag_type)))
			{
				dwNetDbgTypeTmp = debug_type_cmd[i].debug_flag;
				if(dwNetDbgTypeTmp != 0x5a55)
				{
					cmp_flag = 1;
				}
			}
			i++;
		}
		if(cmp_flag == 1)
		{
			if((strncmp(argv[2],"-off",strlen("-off")) == 0) \
				&& (strlen(argv[2]) == strlen("-off")))
			{
				if(flags == 0)
				{
					g_dwNetDbgType &= ~dwNetDbgTypeTmp;
				}
				else if(flags == 1)
				{
					g_dwSerialDbgType &= ~dwNetDbgTypeTmp;
				}
				return 0;
			}
			else
			{
				cmd_usage(cmd,flags);
				return -1;
			}
		}
		else
		{
			if(dwNetDbgTypeTmp == 0x5a55)
			{
				i = 0;
				while(debug_level_cmd[i].debug_level_type != NULL)
				{
					if((strncmp(argv[2],debug_level_cmd[i].debug_level_type,strlen(debug_level_cmd[i].debug_level_type)) == 0) \
					&& (strlen(argv[2]) == strlen(debug_level_cmd[i].debug_level_type)))
					{
						if(flags == 0)
						{
							g_ucTelnetDbgFlg = debug_level_cmd[i].debug_level;
						}
						else if(flags == 1)
						{
							g_ucSerialDbgFlg = debug_level_cmd[i].debug_level;
						}
						return 0;
					}
					i++;
				}
				cmd_usage(cmd,flags);
				return -1;
			}
			else
			{
				cmd_usage(cmd,flags);
				return -1;
			}
		}
	}
	cmd_usage(cmd,flags);
	return -1;
}


int reboot(struct cmd_tbl *cmd, int flags, int argc,char *const argv[])
{
	INT16U usStrLen = 0;
	if(argc == 1)
	{
		Sys_Delay_Reset(3);
		if(flags == 0)
		{
			usStrLen = sprintf(g_szTelnetSndBuf,"\r\nDevice will reboot after 3s\r\n");	
			tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
		}
		else if(flags == 1)
		{
			usStrLen = sprintf(g_szDebugUartSndBuf,"\r\nDevice will reboot after 3s\r\n");						
			debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
		}
		return 0;
	}
	else if(argc == 2)
	{
		if((strncmp(argv[1],"module",strlen("module")) == 0) \
					&& (strlen(argv[1]) == strlen("module")))
		{
			extern void Sys_ModuleReset(void);
			Sys_ModuleReset();
			if(flags == 0)
			{
				usStrLen = sprintf(g_szTelnetSndBuf,"\r\nModule already reboot\r\n");	
				tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
			}
			else if(flags == 1)
			{
				usStrLen = sprintf(g_szDebugUartSndBuf,"\r\nModule already reboot\r\n");						
				debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
			}
			return 0;
		}
		cmd_usage(cmd,flags);
		return -1;
	}
	cmd_usage(cmd,flags);
	return -1;
}

int man(struct cmd_tbl *cmd, int flags, int argc,char *const argv[])
{
	cmd_tbl_s *start = NULL;
	start =	(cmd_tbl_s *)&cmd_tbl_start;
	while(start < &cmd_tbl_end)
	{
		if(start != (cmd_tbl_s *)&cmd_tbl_start)
		{
			cmd_usage(start,flags);
		}
		start++;
	}
	return 0;
}

int set(struct cmd_tbl *cmd, int flags, int argc,char *const argv[])
{
	if(argc == 3)
	{
		if((strncmp(argv[1],"update",strlen("update")) == 0) \
					&& (strlen(argv[1]) == strlen("update")))
		{
			
			return 0;
		}
		return -1;
	}
	return -1;
}



int runtime(struct cmd_tbl *cmd, int flags, int argc,char *const argv[])
{
	INT16U usStrLen = 0;
	INT16U usDay = 0;
	INT16U usHour = 0;
	INT16U usMin = 0;
	INT16U usSec = 0;
	unsigned int runtime;
	runtime = g_stGlobeOps.systimer_ops.get_runtime(NULL);

	usDay  = runtime/86400;
	usHour = runtime%86400;
	usHour = usHour/3600;
	usMin  = runtime%3600;
	usMin  = usMin/60;
	usSec  = runtime%60;
	
	if(flags == 0)
	{
		usStrLen = sprintf(g_szTelnetSndBuf,"\r\nHad Run: %d:%02d:%02d:%02d",usDay,usHour,usMin,usSec);	
		tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
		return 0;
	}
	if(flags == 1)
	{
		usStrLen = sprintf(g_szDebugUartSndBuf,"\r\nHad Run: %d:%02d:%02d:%02d",usDay,usHour,usMin,usSec);						
		debug_uart_data_send(g_szDebugUartSndBuf,usStrLen);	
		return 0;
	}
	return -1;
}


U_BOOT_CMD(debug,3,1,debug_process,"Usage:[debug level -<option>] [debug <cmdtype>] [debug <cmdtype> -off]",
"\r\ndebug level -<option>\
	\r\n\t-r\t\t:DBG_RAW\
	\r\n\t-m\t\t:DBG_MSG\
	\r\n\t-d\t\t:DBG_DETAIL\
	\r\n\t-i\t\t:DBG_INFO\
	\r\n\t-e\t\t:DBG_ERR\
	\r\n\t-n\t\t:DBG_NULL\
	\r\n\r\ndebug <cmdtype> or debug <cmdtype> -off\
	\r\n\trtc\t\t:DBG_RTC_SW\
	\r\n\tcpu\t\t:DBG_CPU_USAGE\
	\r\n\ttcp\t\t:DBG_TCP_SW\
	\r\n\ttelnet\t\t:DBG_TELNET_SW\
	\r\n\tudp\t\t:DBG_UDP_SW\
	\r\n\thttp\t\t:DBG_HTTP_SW\
	\r\n\tgprs\t\t:DBG_GPRS_SW\
	\r\n\tinterface\t:DBG_INTERFACE_SW\
	\r\n\ttask\t\t:DBG_TASK_SW\
	\r\n\tinterflash\t:DBG_INTERFLASH_SW\
	\r\n\tnand\t\t:DBG_SPIFlASH_SW\
	\r\n\tuart\t\t:DBG_UART_SW\
	\r\n\tsys\t\t:DBG_SYS_SW\
	\r\n\tgpio\t\t:DBG_GPIO_SW\
	\r\n\tprotocol\t:DBG_PROT_SW\
	\r\n\tringbuff\t:DBG_RINGBUFF_SW\
	\r\n\trfid\t\t:DBG_RFID_MODULE_SW\
	\r\n\tapp\t\t:DBG_RFID_APP_SW\
	\r\n\tcan\t\t:DBG_CAN_SW\
	\r\n\tcandm\t\t:DBG_CANDM_SW\
	\r\n\trecord\t\t:DBG_RECORD_SW\
	\r\n\tallerror\t:DBG_ALL_ERR_SW\
	\r\n\tall\t\t:DBG_ALL\
	\r\n\t-off\t\t关闭此项调试开关，缺省表示打开对应调试开关\r\n");

U_BOOT_CMD(reboot,2,1,reboot,"Usage:[\"reboot\" or \"reboot module\"]",NULL);
U_BOOT_CMD(man,1,1,man,"Usage:[\"man\"]",NULL);
U_BOOT_CMD(set,3,1,set,"Usage:[\"set <param> <value>\"]",NULL);
U_BOOT_CMD(runtime,1,1,runtime,"Usage:[\"runtime\"]",NULL);

