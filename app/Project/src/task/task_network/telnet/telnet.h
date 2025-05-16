/**
 * @file telnet.h
 * @brief Telnet������ؽṹ����ӿ�����
 * @author �Ż���
 * @date 2024-xx-xx
 */

#ifndef __TELNET_H__
#define __TELNET_H__

#include "includes.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#define ESC '\033'

/**
 * @brief Telnet�û���Ϣ�ṹ��
 */
typedef struct
{
    unsigned char ucState;    // Telnet��¼�׶�״̬(�û���/����׶�)
    unsigned char ucLogin;    // ��¼״̬(�ɹ�/ʧ��)
    unsigned char ucUsrIndex; // �û�����������
    char szUsr[100];         // �û�������
    unsigned char ucPwdIndex; // ������������
    char szPwd[100];         // ���뻺��
} TELNET_USR_INFO;

/**
 * @brief Telnet������ṹ��
 */
typedef struct
{
    char szCmd[MAX_TELNET_CMD_SIZE]; // ���������
} TELNET_CMD_ITEM;

/**
 * @brief Telnet��������ṹ��,����ʵ��������ʷ��¼����
 */
typedef struct
{
    unsigned char ucRead;     // ��ָ��
    unsigned char ucWrite;    // дָ��
    unsigned char ucIndex;    // ��ǰ����
    TELNET_CMD_ITEM stCmdList[MAX_TELNET_CMD_BUFF_LEN]; // ������ʷ��¼����
} TELNET_CMD_LINKLIST;


/**
 * @brief ��־��Ϣ�������ṹ��
 */
typedef struct {
    unsigned short offset;    /**< ƫ���� */
    unsigned short sndpos;    /**< ����λ�� */
    char szMsg[5120];         /**< ��Ϣ���� */
} DbgLogMsgBuff;

/**
 * @brief Telnet��ʼ��
 */
void Telnet_Init(void);

/**
 * @brief Ping����������
 */
void ping_proc(void);

extern DbgLogMsgBuff g_stDbgLogMsgBuff;
extern struct tcp_pcb *g_PCB_Telnet_p;
extern char g_szTelnetSndBuf[1500];
extern INT32U g_dwTelnetKeepAliveTmr;
extern INT8U  g_ucTelnetDbgFlg;
extern INT8U g_ucNeedEchoPrompt;

#endif

