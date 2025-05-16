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
	unsigned int offset;  	// Ƭ��ƫ�ƣ���λword
	unsigned int flg;
}FlashParams;



typedef struct
{
	unsigned int		time;								// ����ʱ��
	unsigned short	time_used;					// ����
	unsigned short	sfid;								// ��ں�
	unsigned char		cmd_len;						// �·������
	unsigned char		szCmd[52];					// �·�����ָ�ʮ������(��Ҫ����˳��Э�黹ԭ��Ȼ��д���ı�)
	unsigned char		ack_len;						// Ӧ�������
	unsigned char		szAck[104];					// Ӧ�����ָ�ʮ������(��Ҫ����˳��Э�黹ԭ��Ȼ��д���ı�)
	unsigned short	crc;								// У��
}WriteUsrRecord;

#define RECORD_SELECT_CONTEXT					0
#define RECORD_SELECT_DATE_INDEX			1
#define RECORD_SELECT_RECORD_INDEX		2
#define RECORD_SELECT_RECOVERY_INDEX	3

typedef struct
{
	unsigned int	select;
	unsigned int  dwPagePos;					// ��־��ʼ����page�������
	unsigned int  dwPageOffsetPos;		// ��־��ʼ����page��ƫ���������
}RecordAddrParams;


typedef struct 
{
	volatile unsigned int  time;										// ��־�����������ʼʱ�伴0ʱ0��0��(����ʱ��)
	volatile unsigned int  dwStartPagePos;					// ��־��ʼ����page�������
	volatile unsigned int  dwStartPageOffsetPos;		// ��־��ʼ����page��ƫ���������
	volatile unsigned int  dwEndPagePos;						// ��־��ֹ����page�������
	volatile unsigned int  dwEndPageOffsetPos;			// ��־��ֹ����page��ƫ���������
}RecordDateIndexParams;


#endif

