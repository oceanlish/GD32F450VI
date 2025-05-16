#ifndef _INF_DEFS_H_
#define _INF_DEFS_H_

#define NET_CONNECTED			0
#define NET_ERROR					1

#define NET_USE_DOMAIN		0x5AA5

typedef enum
{
	INF_TCP_SERVER = 0,
	INF_TCP_CLIENT,
	INF_UART,
	INF_CAN,
	INF_UDP_GROUP,
	INF_UDP,
	INF_INTERFLASH,
	INF_NANDFLASH,
	INF_GPIO,
	NONE_TRANS
}E_INTERFACE_TYPE;

typedef enum
{
	STORE_SYSTEMPARAM = 0,
	STORE_SYSTEMBAKPARAM,
	STORE_RECORDPARAM,
	STORE_RECORDBAKPARAM,
	STORE_APP,
	STORE_APP_BAK,
	NONE_STORE
}E_DATASTORE;


typedef enum
{
	COM0 = 0,
	COM1,
	COM2,
	COM3,
	COM4,
	COM5,
	COM6,
	NONE_COM
}E_COM;

typedef enum
{
	APP_CAN0 = 0,
	APP_CAN1,
	NONE_CAN
}E_CAN;

typedef enum
{
	GPIO_LOW = 0,
	GPIO_HIGH,
	GPIO_TOGGLE
}E_GPIO;

typedef struct  
{
	int inf_type;									
	void *params;						
}llifParams;

typedef struct
{
	int		inf_fd;							
	void	*client_fd;	
}llifDescriptor;


typedef struct
{
	llifDescriptor *pllif_fd;		
}InterfaceHeader;

typedef struct
{
	void *pllif_fd;				
	int		ilen;
	unsigned char	*pbuff;
	void *opt;
}TransData;

typedef struct
{
	void *pllif_fd;			
	int		want_len;
	int		actual_len;
	unsigned char	*pbuff;
	void *opt;
}RevData;

typedef struct
{
	void *pllif_fd;					
	int	index;
	int value;
}GpioParam;

typedef struct
{
	unsigned char index;
	unsigned char dmaEn;
	unsigned char console;
	unsigned char res;
	unsigned int baud;
}UartInitParams;


typedef struct
{
	unsigned char szIp[100];
	unsigned short port;
	unsigned short isDomain;
	int (*connect)(llifDescriptor *);
	int (*disconnect)(llifDescriptor *);
}NetInitParams;

typedef struct
{
	unsigned short baud;
	unsigned char index;
	unsigned char res;
}CanInitParams;

typedef struct
{
	unsigned short rx_sfid;  	
}CanParams;

typedef struct
{
	unsigned short sfid;
	unsigned short len;
	unsigned char *pbuff;	
}SFCanUpgradeParams;


#define FLASH_ERASER_ENABLE					0x01

typedef struct
{
	unsigned int pos;
}FlashInitParams;

typedef struct
{
	unsigned int offset;  	// 片内偏移，单位word
	unsigned int flg;
}FlashParams;



typedef struct
{
	unsigned int		time;								// 格林时间
	unsigned short	time_used;					// 毫秒
	unsigned short	sfid;								// 格口号
	unsigned char		cmd_len;						// 下发命令长度
	unsigned char		szCmd[52];					// 下发具体指令，十六进制(需要根据顺丰协议还原，然后写入文本)
	unsigned char		ack_len;						// 应答命令长度
	unsigned char		szAck[104];					// 应答具体指令，十六进制(需要根据顺丰协议还原，然后写入文本)
	unsigned short	crc;								// 校验
}WriteUsrRecord;

#define RECORD_SELECT_CONTEXT					0
#define RECORD_SELECT_DATE_INDEX			1
#define RECORD_SELECT_RECORD_INDEX		2
#define RECORD_SELECT_RECOVERY_INDEX	3

typedef struct
{
	unsigned int	select;
	unsigned int  dwPagePos;					// 日志起始所在page入口索引
	unsigned int  dwPageOffsetPos;		// 日志起始所在page内偏移入口索引
}RecordAddrParams;


typedef struct 
{
	volatile unsigned int  time;										// 日志产生当天的起始时间即0时0分0秒(格林时间)
	volatile unsigned int  dwStartPagePos;					// 日志起始所在page入口索引
	volatile unsigned int  dwStartPageOffsetPos;		// 日志起始所在page内偏移入口索引
	volatile unsigned int  dwEndPagePos;						// 日志终止所在page入口索引
	volatile unsigned int  dwEndPageOffsetPos;			// 日志终止所在page内偏移入口索引
}RecordDateIndexParams;


#endif

