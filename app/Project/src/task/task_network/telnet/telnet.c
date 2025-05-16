/*!
 * @file  telnet.c
 * @brief Telnet����ʵ���ļ�
 */
#include "main.h"
#include "telnet.h"
#include "cli_cmd.h"
#include "lwip/raw.h"
#include <string.h>
#include "llif_record.h"

/* Telnet������غ궨�� */
#define MAX_TELNET_CMD_SIZE       50     // ����Telnet������󳤶�
#define MAX_TELNET_CMD_BUFF_LEN   10     // Telnet���������

// Telnet��¼״̬����
#define TELNET_LOGIN_SUCCESS      1      // ��¼�ɹ�
#define TELNET_LOGIN_FAIL         0      // ��¼ʧ��

// Telnet��¼�׶ζ���
#define TELNET_USR                0      // �û����׶�
#define TELNET_PWD                1      // ����׶�

// TelnetЭ������״̬����
#define TELNET_DOECHO_CMD         0      // DO ECHO����״̬
#define TELNET_WINSIZE_CMD        1      // ���ڴ�СЭ������״̬
#define TELNET_FLOWCTL_CMD        2      // ������Э������״̬
#define TELNET_WILLECHO_CMD       3      // WILL ECHO����״̬
#define TELNET_DOSUPPRESS_CMD     4      // DO SUPPRESS GO AHEAD����״̬
#define TELNET_CMD_END            99     // ����Э�̽���״̬

INT8U g_ucTelnetCmdState = TELNET_DOECHO_CMD;  // Telnet����Э��״̬,��ʼΪDOECHO����״̬
INT8U g_ucNeedEchoPrompt = 0;                  // �Ƿ���Ҫ��ʾ������ʾ����־,0-����Ҫ,1-��Ҫ
INT8U g_ucLogSuspend = 0;                      // ��־�����ͣ��־,0-����ͣ,1-��ͣ
INT32U g_dwLastTelnetTabRevSysCnt = 0;         // ���һ�ν���Tab����ϵͳʱ��
static int s_ping_sem;                         // ping����ʹ�õ��ź��� 

// Telnet Cmd ��Щ������Telnet�Ự��������������Э�����Ӳ��������Ƿ����û��ԡ����ڴ�С�������Ƶȹ��ܡ�
char g_szTelnetDoEchoCmd[3] = {0xFF, 0xFD, 0x01};     /* command: Do Echo                         FF FD 01 */
char g_szTelnetWinSizeCmd[3] = {0xFF, 0xFD, 0x1F};    /* command: Do Negotiate About window Size  FF FD 1F */
char g_szTelnetFlowCtlCmd[3] = {0xFF, 0xFD, 0x21};    /* command: Do Remote Flow Control          FF FD 21 */
char g_szTelnetWillEchoCmd[3] = {0xFF, 0xFB, 0x01};   /*command: Will Echo                        FF FB 01 */
char g_szTelnetDoSuppressCmd[3] = {0xFF, 0xFB, 0x03}; /*command: Do Suppress Go Ahead             FF FB 03 */

TELNET_USR_INFO g_stTelnetUsrInfo;        // Telnet�û���Ϣ�ṹ��
TELNET_CMD_LINKLIST g_stTelnetCmdList;    // Telnet��������ṹ��,���ڴ洢��ʷ����

struct tcp_pcb *g_PCB_Telnet_p = NULL;    // Telnet TCP���ƿ�ָ��
char g_szTelnetSndBuf[1500] = {0};        // Telnet���ͻ�����
char g_szTelnetCmdBuf[MAX_TELNET_CMD_SIZE] = {0};  // Telnet�������

DbgLogMsgBuff g_stDbgLogMsgBuff;          // ������־��Ϣ������

INT32U g_dwTelnetKeepAliveTmr = 0;        // Telnet���ʱ��
INT32U g_dwTelnetLoginTmr = 0;            // Telnet��¼��ʱ��
INT8U g_ucTelnetDbgFlg = DBG_NULL;        // Telnet���Ա�־
INT8U g_ucTelnetPollNeedShutDownTcp = 0;  // Telnet��ѯ�Ƿ���Ҫ�ر�TCP���ӱ�־
INT8U g_ucTelnetCmdFlg = 0;               // Telnet�����־
INT8U g_szTelnetCmdRevCnt = 0;            // Telnet�ѽ��������ַ�����

ip_addr_t _s_ipaddr;                     // pingĿ��IP��ַ�洢            
ip_addr_t *pingipaddr = &_s_ipaddr;
struct raw_pcb *ping_pcb;                // ping����ʹ�õ�ICMPԭʼ�׽��ֿ��ƿ�
uint32_t g_dwSysTimeNow = 0;             // ��¼ping����ʱ��ϵͳʱ��,���ڼ�������ʱ��
uint32_t g_dwPingCnt = 0;                // ʣ����Ҫ���͵�ping������
#define PING_SND_NUM 4                   // ÿ��ping����Ĭ�Ϸ��͵�ICMP�������������

/**
 * @brief ����ping��
 * @param pcb ICMP���ƿ�
 * @param ipaddr Ŀ��IP��ַ
 */
void ping_send(struct raw_pcb *pcb, ip_addr_t *ipaddr);

/**
 * @brief ��ʾTelnet������ʾ��
 * @param pcb TCP���ƿ�ָ��
 * @param cmd ������������ַ���,ΪNULLʱֻ��ʾ��ʾ��
 * 
 * �ú���������Telnet�ն���ʾ������ʾ��,��ʽΪ:[�û���@��Ʒ�� ~]$
 * ��ʾ���а���ANSIת����������������ɫ:
 * - ESC[1;32m ����Ϊ����ɫ
 * - ESC[0;37m �ָ�ΪĬ�ϰ�ɫ
 */
void Telnet_Display_Cmd_Prompt(struct tcp_pcb *pcb, char *cmd)
{
    unsigned short usStrLen = 0;
    SysParams *pstSysParams = NULL;
    // ��ȡϵͳ����
    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    if (cmd == NULL)
    {
        // ����ʾ��ʾ��
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n[%s@%s %c[1;32m~%c[0;37m]$", g_stTelnetUsrInfo.szUsr, pstSysParams->ProdectInfo.szProductName, ESC, ESC);
    }
    else
    {
        // ��ʾ��ʾ���������������
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n[%s@%s %c[1;32m~%c[0;37m]$%s", g_stTelnetUsrInfo.szUsr, pstSysParams->ProdectInfo.szProductName, ESC, ESC, cmd);
    }
    // ͨ��TCP������ʾ��
    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
}

/**
 * @brief ping�ź����ȴ�
 * @param iTo ��ʱʱ��
 * @return �ȴ����
 */
int ping_sem_pend(int iTo)
{
    return g_stGlobeOps.sem_ops.pend(s_ping_sem, iTo);
}

/**
 * @brief ping�ź����ͷ�
 */
void ping_sem_post(void)
{
    g_stGlobeOps.sem_ops.post(s_ping_sem);
}

/**
 * @brief ICMP��������Ļص�������
 * @param p ���յ������ݰ�
 * @param inp ����ӿ�
 * @param iphdr IPͷ��
 * @return 1-����ɹ�,0-����ʧ��
 */
uint8_t ping_callback(struct pbuf *p, struct netif *inp, struct ip_hdr *iphdr)
{
    char buf[200];                    // ���ڴ洢IP��ַ�ַ���
    unsigned char szIP[4] = {0};      // ���ڴ洢IP��ַ���ֽ�
    INT16U usStrLen;                  // ���ͻ���������
    INT32U dwTimeDiff = 0;            // ����ʱ��
    uint32_t dwSysTimeNow = sys_now();  // ��ȡ��ǰϵͳʱ��
    // ������ݰ������Ƿ�Ϸ�(���ٰ���IPͷ��ICMPͷ)
    if (p->tot_len >= (PBUF_IP_HLEN + 8))
    {
        // ����ԴIP��ַ
        szIP[3] = iphdr->src.addr >> 24;
        szIP[2] = iphdr->src.addr >> 16;
        szIP[1] = iphdr->src.addr >> 8;
        szIP[0] = iphdr->src.addr;
        sprintf((char *)buf, "%u.%u.%u.%u", szIP[0], szIP[1], szIP[2], szIP[3]);
        // ��������ʱ��
        dwTimeDiff = dwSysTimeNow - g_dwSysTimeNow;
        // ����ʱ�Ӹ�ʽ�������Ϣ
        if (dwTimeDiff == 0)
        {
            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n���� %s �Ļظ����ֽ�=%u ʱ��<1ms TTL=%u", buf, p->tot_len - sizeof(struct icmp_echo_hdr), iphdr->_ttl);
        }
        else
        {
            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n���� %s �Ļظ����ֽ�=%u ʱ��=%ums TTL=%u", buf, p->tot_len - sizeof(struct icmp_echo_hdr), dwTimeDiff, iphdr->_ttl);
        }
        // ͨ��telnet������Ӧ��Ϣ
        tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        ping_sem_post();  // �ͷ�ping�ź���
        return 1;
    }
    return 0;
}

/**
 * @brief ping��ʱ����
 */
void ping_timeout(void)
{
    INT16U usStrLen;
    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n����ʱ���޷���������");
    tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
}

/**
 * @brief ping�������ʱ��ʾͳ����Ϣ
 * @param dwSucc ping�ɹ�����
 * @param dwErr pingʧ�ܴ���
 * 
 * �ú���������ping�������ʱ��ʾͳ����Ϣ,����:
 * - Ŀ��IP��ַ
 * - ���͵����ݰ�����
 * - �ɹ����յ����ݰ���
 * - ��ʧ�����ݰ�����������
 */
void ping_proc_end(unsigned int dwSucc, unsigned int dwErr)
{
    INT16U usStrLen;                    // ���ͻ���������
    char buf[100];                      // IP��ַ�ַ���������
    unsigned char szIP[4] = {0};        // IP��ַ���ֽڴ洢����
    // ����Ŀ��IP��ַΪ���ʮ���Ƹ�ʽ
    szIP[3] = (pingipaddr->addr) >> 24;
    szIP[2] = (pingipaddr->addr) >> 16; 
    szIP[1] = (pingipaddr->addr) >> 8;
    szIP[0] = (pingipaddr->addr);
    sprintf((char *)buf, "%u.%u.%u.%u", szIP[0], szIP[1], szIP[2], szIP[3]);
    // ��ʽ��ͳ����Ϣ������
    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n\r\n%s �� Ping ͳ����Ϣ��\r\n\t���ݰ����ѷ��� = %u���ѽ��� = %u����ʧ = %u <%u%% ��ʧ>\r\n", 
        buf, dwSucc + dwErr, dwSucc, dwErr, dwErr * 100 / (dwSucc + dwErr));
    tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
    // ��ʾ������ʾ��
    Telnet_Display_Cmd_Prompt(g_PCB_Telnet_p, NULL);
}

/**
 * @brief ping�����̺���
 * 
 * �ú���ʵ��ping�������Ҫ�߼�:
 * - ѭ������ping����ֱ������Ϊ0
 * - ÿ��ping����ȴ���Ӧ3��
 * - ͳ�Ƴɹ���ʧ�ܴ���
 * - ����ping����������1��
 */
void ping_proc(void)
{
    OS_TICK iTo = 0;                                    // �ź����ȴ���ʱʱ��
    unsigned int dwSystick = 0;                         // ϵͳʱ�����
    unsigned int dwPingSuccCnt = 0;                     // ping�ɹ�����
    unsigned int dwPingErrCnt = 0;                      // pingʧ�ܼ���
    
    ping_sem_pend(iTo);                                // ��ʼ�ȴ��ź���

    // ���ping���ƿ��Ƿ���Ч
    if (ping_pcb == NULL || pingipaddr == NULL) {
        // ��ʾ������Ϣ
        INT16U usStrLen = sprintf(g_szTelnetSndBuf, "\r\nPing��ʼ��ʧ��");
        tcp_write(g_PCB_Telnet_p, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        Telnet_Display_Cmd_Prompt(g_PCB_Telnet_p, NULL);
        return;
    }

    while (g_dwPingCnt > 0)                            // ѭ��ֱ���������ping����
    {
        ping_send(ping_pcb, pingipaddr);               // ����Ping����
        iTo = 3000;                                    // ����3�볬ʱ
        dwSystick = sys_now();                         // ��¼����ʱ��
        if (ping_sem_pend(iTo) != OS_ERR_NONE)         // �ȴ�ping��Ӧ
        {
            ping_timeout();                            // ��ʱ����
            dwPingErrCnt++;                            // ʧ�ܼ�����1
        }
        else
        {
            dwPingSuccCnt++;                          // �ɹ�������1
            dwSystick = sys_now() - dwSystick;        // ����ʵ����ʱ
            if (dwSystick < 1000)                     // �����ʱС��1��
            {
                g_stGlobeOps.systimer_ops.sleep(1000 - dwSystick);  // ����ȴ���1��
            }
        }
        g_dwPingCnt--;                                // ʣ��ping������1
        if (g_dwPingCnt == 0)                         // ����ping�������
        {
            ping_proc_end(dwPingSuccCnt, dwPingErrCnt);  // ��ʾͳ�ƽ��
            dwPingSuccCnt = 0;                        // ���ü�����
            dwPingErrCnt = 0;
        }
    }
    iTo = 0;                                          // ���ó�ʱʱ��
}

/**
 * @brief ICMP���ƿ��ʼ��
 * 
 * �ú������ڳ�ʼ��ICMPԭʼ�׽��ֿ��ƿ�,����ping����
 * 
 * @return 0-��ʼ���ɹ�, 1-��ʼ��ʧ��
 * 
 * @note 
 * - ʹ��raw_new()����ICMPԭʼ�׽���
 * - Ŀǰδ���ûص�����ע���IP�󶨹���
 */
uint8_t icmp_pcb_init(void)
{
    // ����ICMPԭʼ�׽��ֿ��ƿ�
    ping_pcb = raw_new(IP_PROTO_ICMP);
    // �����ƿ��Ƿ񴴽��ɹ�
    if (!ping_pcb)
        return 1; 
    // raw_recv(ping_pcb,raw_callback,NULL);  //ע��ص�����
    // raw_bind(ping_pcb,IP_ADDR_ANY);        //�󶨱���IP��ַ  ����ANYIP
    return 0;
}

/**
 * @brief ׼��ICMP�����������ݰ�
 * @param iecho ICMPͷ���ṹ��ָ��
 * @param ping_size ping���ݰ��ܴ�С
 * 
 * �ú����������ICMP����������ĸ����ֶ�:
 */
void ping_prepare_echo(struct icmp_echo_hdr *iecho, uint16_t ping_size)
{
    size_t i;
    char send_c = 'a';
    // size_t data_len=ping_size-sizeof(struct icmp_echo_hdr);
    
    /* ����ICMPͷ���ֶ� */
    ICMPH_TYPE_SET(iecho, ICMP_ECHO);    // ��������Ϊ��������
    ICMPH_CODE_SET(iecho, 0);            // ���ô���Ϊ0
    iecho->chksum = 0;                   // У��ͳ�ʼ��Ϊ0
    iecho->id = 0x01;                    // ��ʶ����Ϊ0x01
    iecho->seqno = 0x8418;               // ���к���Ϊ0x8418
    /* ���ICMP���ݲ��� */
    for (i = 0; i < ping_size; i++)
    {
        ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = send_c;  // �������
        send_c = send_c + 1;     // �ַ�����
        if (send_c == 'z')       // ����z�����´�a��ʼ
        {
            send_c = 'a';
        }
    }
    // iecho->chksum=inet_chksum(iecho,ping_size);
}

/**
 * @brief ���첢����ICMP��������(ping)���ݰ�
 * @param pcb ICMP���ƿ�
 * @param ipaddr Ŀ��IP��ַ
 * 
 */
void ping_send(struct raw_pcb *pcb, ip_addr_t *ipaddr)
{
    struct pbuf *p;                    // pbuf�������ṹ��
    struct icmp_echo_hdr *iecho;       // ICMPͷ���ṹ��ָ��
    // telnet���ӶϿ�ʱֹͣping
    if (g_PCB_Telnet_p == NULL)
    {
        g_dwPingCnt = 1;              // ����ʣ��ping����Ϊ1,ʹping�����������
    }
    // ����pbuf������,��СΪICMPͷ��+32�ֽ�����
    p = pbuf_alloc(PBUF_IP, 32 + sizeof(struct icmp_echo_hdr), PBUF_RAM); // ����pbuf�ṹ
    if (!p)
    {
        return;                        // ����ʧ��ֱ�ӷ���
    }
    // ���pbuf�Ƿ�Ϊ��������������
    if (p->len == p->tot_len && p->next == NULL)
    {
        iecho = (struct icmp_echo_hdr *)p->payload;  // ��ȡ������ָ��
        ping_prepare_echo(iecho, 32);                // ��дICMP�ײ����ֶ�
        raw_sendto(pcb, p, ipaddr);                 // �ײ㷢��
    }
    g_dwSysTimeNow = sys_now();                     // ��¼����ʱ��
    pbuf_free(p);                                   // �ͷ�pbuf
}

/**
 * @brief ����Telnet�����ʷ�������
 * @param cmd Ҫ����������ַ���
 * 
 */
static void Insert_Telnet_Cmd(char *cmd)
{
    // �������һ�����������λ��
    int lastCmdIndex = (g_stTelnetCmdList.ucWrite + MAX_TELNET_CMD_BUFF_LEN - 1) % MAX_TELNET_CMD_BUFF_LEN;
    
    // ����Ƿ������һ�������ظ�
    if ((strlen(g_stTelnetCmdList.stCmdList[lastCmdIndex].szCmd) == strlen(cmd)) &&
        (0 == memcmp(g_stTelnetCmdList.stCmdList[lastCmdIndex].szCmd, cmd, strlen(cmd))))
    {
        // �ظ�����,�����µ�ǰ����
        g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
        return;
    }

    // ��黺�����Ƿ�����
    if ((g_stTelnetCmdList.ucWrite + 1) % MAX_TELNET_CMD_BUFF_LEN == g_stTelnetCmdList.ucRead)
    {
        // ��������,������������ƶ���ָ��
        memset(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucRead].szCmd, 0, 
               sizeof(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucRead].szCmd));
        g_stTelnetCmdList.ucRead = (g_stTelnetCmdList.ucRead + 1) % MAX_TELNET_CMD_BUFF_LEN;
    }

    // ���������дָ��λ��
    memcpy(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucWrite].szCmd, cmd, strlen(cmd));
    
    // ����дָ��͵�ǰ����
    g_stTelnetCmdList.ucWrite = (g_stTelnetCmdList.ucWrite + 1) % MAX_TELNET_CMD_BUFF_LEN;
    g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
}

/**
 * @brief ������һ��Telnet����
 * @param cmd ���ڴ洢�ҵ�������
 * @return 0xFF-�ѵ�����������; 1-�ҵ�����; 0-������
 */
static INT8U Find_Telnet_Prev_Cmd(char *cmd)
{
    // �����������Ƿ�Ϊ��
    if (g_stTelnetCmdList.ucRead != g_stTelnetCmdList.ucWrite)
    {
        // �ѵ������������
        if (g_stTelnetCmdList.ucRead == g_stTelnetCmdList.ucIndex)
        {
            memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd, 
                strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
            return 0xFF;
        }
        // ��������ָ����һ������
        if (g_stTelnetCmdList.ucIndex == 0)
        {
            g_stTelnetCmdList.ucIndex = MAX_TELNET_CMD_BUFF_LEN - 1;
        }
        else
        {
            g_stTelnetCmdList.ucIndex--;
        }
        // �����ҵ�������
        memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
            strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
        return 1;
    }
    return 0;
}

/**
 * @brief ������һ��Telnet����
 * @param cmd ���ڴ洢�ҵ�������
 * @return 0xFF-�ѵ�����������; 1-�ҵ�����; 0-������
 */
static INT8U Find_Telnet_Next_Cmd(char *cmd)
{
    // �����������Ƿ�Ϊ��
    if (g_stTelnetCmdList.ucRead != g_stTelnetCmdList.ucWrite)
    {
        // �ѵ������µ�����
        if (((g_stTelnetCmdList.ucIndex + 1) % MAX_TELNET_CMD_BUFF_LEN == g_stTelnetCmdList.ucWrite) ||
            (g_stTelnetCmdList.ucIndex == g_stTelnetCmdList.ucWrite))
        {
            memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
                strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
            return 0xFF;
        }
        // ��������ָ����һ������
        g_stTelnetCmdList.ucIndex = (g_stTelnetCmdList.ucIndex + 1) % MAX_TELNET_CMD_BUFF_LEN;
        // �����ҵ�������
        memcpy(cmd, g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd,
            strlen(g_stTelnetCmdList.stCmdList[g_stTelnetCmdList.ucIndex].szCmd));
        return 1;
    }
    return 0;
}

/**
 * @brief ����Telnet����
 * @param cmd �����ַ���
 * @param pcb TCP���ƿ�
 */
static void telnet_cmd_process(char *cmd, struct tcp_pcb *pcb)
{
    int i = 0;
    unsigned char ucFlg = 0;  // ������־
    unsigned short usStrLen = 0;
    arg_info tmp_arg_info = {0};

    // ����ȫ�ո�����
    if (cmd[0] == ' ')
    {
        for (i = 0; i < strlen(cmd); i++)
        {
            if (cmd[i] != ' ')
            {
                break;
            }
        }
        if (i == strlen(cmd))
        {
            ucFlg = 1;
            if (g_ucLogSuspend == 0)
            {
                g_ucLogSuspend = 1;
                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;33m�ն�����ͣ%c[0;37m", ESC, ESC);
                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
            }
            else
            {
                g_ucLogSuspend = 0;
                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;33m�ն��ѻָ�%c[0;37m", ESC, ESC);
                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
            }
            return;
        }
    }

    // ���������ʷ��¼
    Insert_Telnet_Cmd(cmd);

    // ������ִ������
    if (cmd_parse_info(cmd, &tmp_arg_info) >= 1)
    {
        if (find_cmd(tmp_arg_info.argv[0], &tmp_arg_info, 0) != NULL)
        {
            ucFlg = 1;
        }
    }

    // ��ʾ��Ч������ʾ
    if (0 == ucFlg)
    {
        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31m��Ч����: %s%c[0;37m", ESC, g_szTelnetCmdBuf, ESC);
        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
    }
}

/**
 * @brief ɾ��Զ����������ʾ���ַ�
 * @param pcb TCP���ƿ�
 * @param size Ҫɾ�����ַ���
 * 
 * ͨ�������˸�Ϳո�ʵ��ɾ��Ч��
 */
static void Telnet_Delete_Remote_Cmdline(struct tcp_pcb *pcb, INT8U size)
{
    INT8U i = 0;
    INT16U usStrLen = 0;

    if (size > 0)
    {
        for (i = 0; i < size; i++)
        {
            sprintf(&g_szTelnetSndBuf[usStrLen], "\b \b");
            usStrLen += 3;
        }
        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
        tcp_output(pcb);
    }
}

/**
 * @brief Telnet�����˳�����
 * @param pcb TCP���ƿ�
 * @param p ���ݰ�������
 * @return ������
 * 
 * ������Դ���ر�TCP����
 */
static err_t telnet_exit(struct tcp_pcb *pcb, struct pbuf *p)
{
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    pbuf_free(p);
    g_PCB_Telnet_p = NULL;
    if (pcb->state == ESTABLISHED)
    {
        tcp_close(pcb);
        return ERR_ABRT;
    }
    else
    {
        return tcp_close(pcb);
    }
}

static err_t telnet_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    cmd_tbl_s *start = NULL;
    INT16U usRevLen = 0, i = 0, k = 0;
    INT16U usStrLen = 0;
    INT8U ucRet = 0, ucFind = 0;
    char szCmdTmp[50] = {0};
    char szCmdTmp1[50] = {0};
    unsigned char *pData = NULL;
    SysParams *pstSysParams = NULL;

    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);

    if (p != NULL)
    {
        if (g_PCB_Telnet_p != pcb)
        {
            // TCP END
            if (pcb != NULL)
            {
                // tcp_close(pcb);
                // tcp_output(pcb);
                tcp_abort(pcb);
            }

            pbuf_free(p);
            err = ERR_OK;
            return err;
        }
    }
    else
    {
        DbgLog(DBG_TCPSERVER_SW, DBG_ERR, DBG_COLOR_NULL, DBG_TS_DIS, "2 pcb %x %x state %u err %u Len 0\r\n", pcb, g_PCB_Telnet_p, pcb->state, err);
        if (pcb != NULL)
        {
            // tcp_close(pcb);
            // tcp_output(pcb);
            if (pcb->state == CLOSE_WAIT)
            {
                tcp_close(pcb);
            }
            else if (pcb->state == TIME_WAIT)
            {
                tcp_abort(pcb);
            }
        }
        err = ERR_OK;
        return err;
    }

    // printf("Telnet Rev Pcb<0x%x>  err %d  p<0x%x>\r\n",pcb,err,p);
    if ((err == ERR_OK) && (p != NULL))
    {
        tcp_recved(pcb, p->tot_len);
        usRevLen = p->tot_len;

        if (usRevLen == 0)
        {
            g_szTelnetCmdRevCnt = 0;
            DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send 0 Byte For FIN!\r\n");
            return telnet_exit(pcb, p);
        }
        // DbgLog(DBG_TELNET_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_DIS,"\r\n");
        // DbgLog(DBG_TELNET_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"Telnet Rev Interval %ds\r\n", (g_stGlobeOps.systimer_ops.get_runtime(NULL);-g_dwTelnetKeepAliveTmr));
        g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
        for (q = p; q != NULL; q = q->next)
        {

            pData = q->payload;

            if (*pData == 0xFF)
            {
                /*for(i=0;i<q->len;i++)
                {
                    printf("0x%x ",*(pData+i));
                }
                printf("\r\n");
                */
                switch (g_ucTelnetCmdState)
                {
                case TELNET_DOECHO_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_WINSIZE_CMD;
                        tcp_write(pcb, g_szTelnetWinSizeCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_WINSIZE_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_FLOWCTL_CMD;
                        tcp_write(pcb, g_szTelnetFlowCtlCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_FLOWCTL_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_WILLECHO_CMD;
                        tcp_write(pcb, g_szTelnetWillEchoCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_WILLECHO_CMD:
                {
                    if (q->len >= 3)
                    {
                        g_ucTelnetCmdState = TELNET_DOSUPPRESS_CMD;
                        tcp_write(pcb, g_szTelnetDoSuppressCmd, 3, TCP_WRITE_FLAG_COPY);
                        tcp_output(pcb);
                    }
                }
                break;
                case TELNET_DOSUPPRESS_CMD:
                {
                    if (((q->len) >= 3) && (*(pData + (q->len) - 3) == 0xFF) && (*(pData + (q->len) - 2) == 0xFD) && (*(pData + (q->len) - 1) == 0x03))
                    {
                        g_ucTelnetCmdState = TELNET_CMD_END;
                        usStrLen = sprintf(g_szTelnetSndBuf, "\r\n\r\n           Welcome to %s Application Embedded uC/OS-III Environment           \r\n\r\n", pstSysParams->ProdectInfo.szProductName);
                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                        usStrLen = sprintf(g_szTelnetSndBuf, "%s login:", pstSysParams->ProdectInfo.szProductName);
                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                    }
                }
                break;
                default:
                {
                    if (((q->len) == 2) && (*(pData + 1) == 0xF1))
                    {
                        DbgLog(DBG_TELNET_SW, DBG_INFO, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet Cmd: No Operation\r\n");
                    }
                }
                break;
                }

                pbuf_free(p);
                return ERR_OK;
            }
            if (q->len == 3)
            {
                if ((*(pData) == 0x1B) && (*(pData + 1) == 0x5B))
                {
                    switch (*(pData + 2))
                    {
                    case 0x41:
                    {
                        // up
                        // printf("up\r\n");

                        memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                        ucRet = Find_Telnet_Prev_Cmd(g_szTelnetCmdBuf);
                        if (1 == ucRet)
                        {
                            Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                            if ((g_ucTelnetDbgFlg != DBG_NULL) && (1 == g_ucNeedEchoPrompt))
                            {
                                g_ucNeedEchoPrompt = 0;
                                Telnet_Display_Cmd_Prompt(pcb, NULL);
                            }
                            tcp_write(pcb, g_szTelnetCmdBuf, strlen(g_szTelnetCmdBuf), TCP_WRITE_FLAG_COPY);
                            tcp_output(pcb);
                        }
                        else if (0xFF == ucRet)
                        {
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                        }
                    }
                    break;
                    case 0x42:
                    {
                        // down
                        // printf("down\r\n");

                        memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                        ucRet = Find_Telnet_Next_Cmd(g_szTelnetCmdBuf);
                        if (1 == ucRet)
                        {
                            Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                            if ((g_ucTelnetDbgFlg != DBG_NULL) && (1 == g_ucNeedEchoPrompt))
                            {
                                g_ucNeedEchoPrompt = 0;
                                Telnet_Display_Cmd_Prompt(pcb, NULL);
                            }
                            tcp_write(pcb, g_szTelnetCmdBuf, strlen(g_szTelnetCmdBuf), TCP_WRITE_FLAG_COPY);
                            tcp_output(pcb);
                        }
                        else if (0xFF == ucRet)
                        {
                            g_szTelnetCmdRevCnt = strlen(g_szTelnetCmdBuf);
                        }
                    }
                    break;
                    case 0x43:
                    {
                        // right
                        // printf("right\r\n");
                    }
                    break;
                    case 0x44:
                    {
                        // left
                        // printf("left\r\n");
                    }
                    break;
                    default:
                        break;
                    }
                    pbuf_free(p);
                    return ERR_OK;
                }
            }

            for (i = 0; i < q->len; i++)
            {
                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_DIS,"0x%x",*(pData+i));
                if (*(pData + i) == 0x09)
                {
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_SUCCESS)
                    {
                        if (g_szTelnetCmdRevCnt != 0)
                        {
                            memcpy(szCmdTmp, g_szTelnetCmdBuf, g_szTelnetCmdRevCnt);
                            // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Need Find %s\r\n",szCmdTmp);

                            if ((sys_now() - g_dwLastTelnetTabRevSysCnt) < 300)
                            {
                                //---double tab���з��ϵ�ָ����ʾ
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB Cmd\r\n");
                                start = (cmd_tbl_s *)&cmd_tbl_start;
                                while (start < &cmd_tbl_end)
                                {
                                    if (memcmp(start->name, szCmdTmp, strlen(szCmdTmp)) == 0)
                                    {
                                        if (0 == ucFind)
                                        {
                                            usStrLen = sprintf(g_szTelnetSndBuf, "\r\n");
                                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                            ucFind = 1;
                                        }
                                        usStrLen = sprintf(g_szTelnetSndBuf, "%s\r\n", start->name);
                                        tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                    }
                                    start++;
                                }
                                if (1 == ucFind)
                                {
                                    Telnet_Display_Cmd_Prompt(pcb, szCmdTmp);
                                }
                                else
                                {
                                    if (g_dwPingCnt > 1)
                                    {
                                        //    ׼��ֹͣPing����
                                        g_dwPingCnt = 1;
                                    }
                                }
                            }
                            else
                            {
                                //---single tab����
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB Cmd\r\n");
                                start = (cmd_tbl_s *)&cmd_tbl_start;
                                while (start < &cmd_tbl_end)
                                {
                                    if (memcmp(start->name, szCmdTmp, strlen(szCmdTmp)) == 0)
                                    {
                                        if (0 == ucFind)
                                        {
                                            memcpy(szCmdTmp1, start->name, strlen(start->name));
                                        }
                                        else
                                        {
                                            for (k = 0; k < strlen(szCmdTmp1); k++)
                                            {
                                                if (szCmdTmp1[k] == start->name[k])
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

                                if (ucFind >= 1)
                                {
                                    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find --> %s\r\n",szCmdTmp1);
                                    Telnet_Delete_Remote_Cmdline(pcb, g_szTelnetCmdRevCnt);
                                    memset(g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
                                    memcpy(g_szTelnetCmdBuf, szCmdTmp1, strlen(szCmdTmp1));
                                    g_szTelnetCmdRevCnt = strlen(szCmdTmp1);
                                    usStrLen = sprintf(g_szTelnetSndBuf, "%s", g_szTelnetCmdBuf);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                }
                                else
                                {
                                    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Nothing Find...\r\n");
                                }
                            }
                            g_dwLastTelnetTabRevSysCnt = sys_now();
                        }
                        else
                        {
                            if ((sys_now() - g_dwLastTelnetTabRevSysCnt) < 300)
                            {
                                //---double tab���з��ϵ�ָ����ʾ
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Double TAB\r\n");

                                if (g_dwPingCnt > 1)
                                {
                                    //    ׼��ֹͣPing����
                                    g_dwPingCnt = 1;
                                }
                            }
                            else
                            {
                                //---single tab����
                                // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_YELLOW,DBG_TS_DIS,"Find Single TAB\r\n");
                            }
                            g_dwLastTelnetTabRevSysCnt = sys_now();
                        }
                    }
                }
                else if ((*(pData + i) >= 0x20) && (*(pData + i) <= 0x7E))
                {
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR) // ��¼������/��ʾ
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex < 99)
                            {
                                g_stTelnetUsrInfo.szUsr[g_stTelnetUsrInfo.ucUsrIndex] = *(pData + i);
                                g_stTelnetUsrInfo.ucUsrIndex++;
                                // printf("++++++++++++++++++-->%s\r\n",g_stTelnetUsrInfo.szUsr);
                                usStrLen = sprintf(g_szTelnetSndBuf, "%c", *(pData + i));
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                        }
                        else // ��������/��ʾ
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex < 99)
                            {
                                g_stTelnetUsrInfo.szPwd[g_stTelnetUsrInfo.ucPwdIndex] = *(pData + i);
                                g_stTelnetUsrInfo.ucPwdIndex++;
                                // printf("++++++++++++++++++-->%s\r\n",g_stTelnetUsrInfo.szPwd);

                                usStrLen = sprintf(g_szTelnetSndBuf, "*");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                        }
                    }
                    else // ָ������/��ʾ
                    {
                        if (g_szTelnetCmdRevCnt < (MAX_TELNET_CMD_SIZE - 1))
                        {
                            g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = *(pData + i);
                            g_szTelnetCmdRevCnt++;
                            usStrLen = sprintf(g_szTelnetSndBuf, "%c", *(pData + i));
                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                        }
                    }
                }
                else if (*(pData + i) == 0x08)
                {
                    // delete
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR) // ��¼��ɾ��/��ʾ
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex > 0)
                            {
                                g_stTelnetUsrInfo.ucUsrIndex--;
                                g_stTelnetUsrInfo.szUsr[g_stTelnetUsrInfo.ucUsrIndex] = 0;
                                // printf("------------>%s\r\n",g_stTelnetUsrInfo.szUsr);
                                usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                            }
                        }
                        else // ����ɾ��/��ʾ
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex > 0)
                            {
                                g_stTelnetUsrInfo.ucPwdIndex--;
                                g_stTelnetUsrInfo.szPwd[g_stTelnetUsrInfo.ucPwdIndex] = 0;
                                // printf("------------>%s\r\n",g_stTelnetUsrInfo.szPwd);
                                usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                            }
                        }
                    }
                    else // ָ��ɾ��/��ʾ
                    {
                        if (g_szTelnetCmdRevCnt > 0)
                        {
                            g_szTelnetCmdRevCnt--;
                            g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = 0;
                            g_stTelnetCmdList.ucIndex = g_stTelnetCmdList.ucWrite;
                            usStrLen = sprintf(g_szTelnetSndBuf, "\b \b");
                            tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                        }
                    }
                }
                else if (*(pData + i) == 0x0D)
                {
                    g_ucTelnetCmdFlg = 0x0D;
                    //---Cmd Enter End
                    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
                    {
                        //---��¼���������
                        if (g_stTelnetUsrInfo.ucState == TELNET_USR)
                        {
                            if (g_stTelnetUsrInfo.ucUsrIndex == 0)
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%s login:", pstSysParams->ProdectInfo.szProductName);
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\npassword:");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                g_stTelnetUsrInfo.ucState = TELNET_PWD;
                            }
                        }
                        //---�����������
                        else
                        {
                            if (g_stTelnetUsrInfo.ucPwdIndex == 0)
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\npassword:");
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                            }
                            else
                            {
                                //---��¼�ɹ�

                                if ((strlen(g_stTelnetUsrInfo.szUsr) == 4) && (strlen(g_stTelnetUsrInfo.szPwd) == 9) &&
                                    (strcmp(g_stTelnetUsrInfo.szUsr, "root") == 0) && (strcmp(g_stTelnetUsrInfo.szPwd, "Hik12345+") == 0))
                                {
                                    g_stTelnetUsrInfo.ucLogin = TELNET_LOGIN_SUCCESS;
                                    g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
                                    Telnet_Display_Cmd_Prompt(pcb, NULL);
                                }
                                //---��¼ʧ��
                                else
                                {
                                    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31musr name or password error! Please try again!%c[0;37m", ESC, ESC);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                                    usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%s login:", pstSysParams->ProdectInfo.szProductName);
                                    tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);
                                    memset(&g_stTelnetUsrInfo, 0, sizeof(g_stTelnetUsrInfo));
                                }
                            }
                        }
                    }
                    //---ָ���������
                    else
                    {
                        g_szTelnetCmdBuf[g_szTelnetCmdRevCnt] = 0;
                        g_szTelnetCmdRevCnt = 0;
                        if (strlen(g_szTelnetCmdBuf) > 0)
                        {
                            // DbgLog(DBG_TELNET_SW,DBG_ERR,DBG_COLOR_GREEN,DBG_TS_EN,"Telnet Cmd: [%s]\r\n",g_szTelnetCmdBuf);
                            if (0 == strcmp(g_szTelnetCmdBuf, "quit"))
                            {
                                usStrLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;32mTelnet Cmd: [%s]%c[0;37m", ESC, g_szTelnetCmdBuf, ESC);
                                tcp_write(pcb, g_szTelnetSndBuf, usStrLen, TCP_WRITE_FLAG_COPY);

                                DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send [quit] Cmd!\r\n");
                                return telnet_exit(pcb, p);
                            }
                            else
                            {
                                telnet_cmd_process(g_szTelnetCmdBuf, pcb);
                            }
                        }

                        g_ucNeedEchoPrompt = 0;
                        Telnet_Display_Cmd_Prompt(pcb, NULL);
                    }
                }
                else if ((*(pData + i) == 0x0A) && (g_ucTelnetCmdFlg == 0x0D))
                {
                    g_ucTelnetCmdFlg = 0x00;
                }
                else if (*(pData + i) == 0x03)
                {
                    DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Client Send 0x03 Cmd!\r\n");
                    return telnet_exit(pcb, p);
                }
            }
        }

        tcp_output(pcb);
        if (p != NULL)
        {
            pbuf_free(p);
        }
    }
    return ERR_OK;
}

err_t Telnet_Sent_Func(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    // printf("-------------->Telnet_Sent_Func%x %x\r\n",tpcb,g_PCB_Telnet_p);
    tcp_output(tpcb);
    tcp_abort(tpcb);
    g_PCB_Telnet_p = NULL;
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    return ERR_ABRT;
}

/**
 * @brief tcp poll callback function
 * @param arg: pointer to an argument to be passed to callback function
 * @param pcb: pointer on tcp_pcb structure
 * @retval err_t
 */
static err_t telnet_poll(void *arg, struct tcp_pcb *pcb)
{
    INT16U usLen = 0;
    // OS_ERR  err;
    // static int iTest = 0;
    // iTest++;
    // DbgLog(DBG_TELNET_SW,DBG_DETAIL,DBG_COLOR_NULL,DBG_TS_EN,"Telnet Poll: pcb<0x%x> state %d\r\n",pcb,pcb->state);
    // printf("Telnet Tcp Had %d Bytes Free,g_PCB_Telnet_p 0x%x pcb 0x%x\r\n",tcp_sndbuf(g_PCB_Telnet_p),g_PCB_Telnet_p,pcb);

    if (pcb->state == CLOSE_WAIT)
    {
        // if((g_stGlobeOps.systimer_ops.get_runtime(NULL)-s_dwTcpServerTmr)>3)
        {
            // g_tcpaccept_pcb = NULL;
            DbgLog(DBG_TCPSERVER_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet Connection With Close Method! Because Tcp State is Close Wait!\r\n");
            tcp_close(pcb);
            return ERR_OK;
        }
    }

    if (g_ucTelnetPollNeedShutDownTcp == 1)
    {
        g_ucTelnetPollNeedShutDownTcp = 0;
        // tcp_output(pcb);
        tcp_close(pcb);
        DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Detect Poll Shut Down Flag is TRUE!\r\n");
        return ERR_OK;
    }
    /*if(pcb->state == CLOSE_WAIT)
    {
        g_PCB_Telnet_p = NULL;
        g_ucTelnetDbgFlg = DBG_NULL;
        g_dwNetDbgType = DBG_NULL;
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Shut Down Telnet TCP Connection With Close Method! Because Tcp State is Close Wait!\r\n");
        return tcp_close(pcb);
    }*/
    if (g_stTelnetUsrInfo.ucLogin == TELNET_LOGIN_FAIL)
    {
        if (g_stGlobeOps.systimer_ops.get_runtime(NULL) > g_dwTelnetLoginTmr)
        {
            if ((g_stGlobeOps.systimer_ops.get_runtime(NULL) - g_dwTelnetLoginTmr) > 60)
            {
                DbgLog(DBG_TELNET_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "Shut Down Telnet TCP Connection! Because Login Time Out!\r\n");
                g_PCB_Telnet_p = NULL;
                g_dwNetDbgType = DBG_NULL;
                g_ucTelnetDbgFlg = DBG_NULL;
                if (pcb->state == ESTABLISHED)
                {
                    usLen = sprintf(g_szTelnetSndBuf, "\r\n%c[1;31mLogin time out after 60 seconds!%c[0;37m", ESC, ESC);
                    tcp_write(pcb, g_szTelnetSndBuf, usLen, TCP_WRITE_FLAG_COPY);
                    tcp_output(pcb);
                    g_ucTelnetPollNeedShutDownTcp = 1;
                    // OSTimeDlyHMSM(0,0,0,100, OS_OPT_TIME_PERIODIC, &err);
                    // tcp_abort(pcb);
                    return ERR_OK;
                }
            }
        }
        else
        {
            g_dwTelnetLoginTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
        }
    }

    if (g_stGlobeOps.systimer_ops.get_runtime(NULL) > g_dwTelnetKeepAliveTmr)
    {
        if ((g_stGlobeOps.systimer_ops.get_runtime(NULL) - g_dwTelnetKeepAliveTmr) > 60 * 10)
        {
            g_PCB_Telnet_p = NULL;
            g_ucTelnetDbgFlg = DBG_NULL;
            g_dwNetDbgType = DBG_NULL;
            return tcp_close(pcb);
        }
    }
    else
    {
        g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    }

    return ERR_OK;
}

void telnet_conn_err(void *arg, err_t err)
{
    struct tcp_pcb *TcpPcbTmp = (struct tcp_pcb *)arg;

    DbgLog(DBG_TCP_SW | DBG_ALL_ERR_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "g_PCB_Telnet_p 0x%x Telnet TCP PCB<0x%x> Connect Error<%d>\r\n", g_PCB_Telnet_p, TcpPcbTmp, err);
    /*if(g_PCB_Telnet_p->state == ESTABLISHED)
    {
        tcp_output(g_PCB_Telnet_p);
        tcp_abort(g_PCB_Telnet_p);
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Telnet TCP Connect Error! Now Shut Down Telnet TCP Connection With Abort Method!\r\n");
    }
    else
    {
        DbgLog(DBG_TELNET_SW|DBG_ALL_ERR_SW,DBG_ERR,DBG_COLOR_RED,DBG_TS_EN,"Telnet TCP Connect Error! Now Shut Down Telnet TCP Connection With Close Method!\r\n");
    if(g_PCB_Telnet_p != NULL)
        tcp_close(g_PCB_Telnet_p);
    }*/
}

static err_t telnet_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{

    // DbgLog(DBG_HTTP_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"http_sent %x %u\r\n",(unsigned int)tpcb,len);
    // DbgLog(DBG_HTTP_SW,DBG_INFO,DBG_COLOR_YELLOW,DBG_TS_EN,"http_sent %x %u %u\r\n",hs->pcb,hs->dwTotalSize,hs->dwHadSendSize);

    return ERR_OK;
}

/**
 * @brief  callback function on TCP connection setup ( on port 23)
 * @param  arg: pointer to an argument structure to be passed to callback function
 * @param  pcb: pointer to a tcp_pcb structure
 * &param  err: Lwip stack error code
 * @retval err
 */
static err_t telnet_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    // INT16U usLen = 0;
    if (g_PCB_Telnet_p != NULL)
    {
        // tcp_output(g_PCB_Telnet_p);
        tcp_close(g_PCB_Telnet_p);
    }
    memset(&g_stDbgLogMsgBuff, 0, sizeof(g_stDbgLogMsgBuff));
    g_ucTelnetDbgFlg = DBG_NULL;
    g_dwNetDbgType = DBG_NULL;
    g_dwTelnetLoginTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    g_dwTelnetKeepAliveTmr = g_stGlobeOps.systimer_ops.get_runtime(NULL);
    g_szTelnetCmdRevCnt = 0;
    g_ucTelnetPollNeedShutDownTcp = 0;
    memset(&g_szTelnetCmdBuf, 0, sizeof(g_szTelnetCmdBuf));
    memset(&g_stTelnetUsrInfo, 0, sizeof(g_stTelnetUsrInfo));
    memset(&g_stTelnetCmdList, 0, sizeof(g_stTelnetCmdList));

    tcp_recv(pcb, telnet_recv);
    tcp_sent(pcb, telnet_sent);
    tcp_err(pcb, telnet_conn_err);
    tcp_poll(pcb, telnet_poll, 0);
    // pcb->flags |= ((u8_t)0x40U);// TF_NODELAY

    g_PCB_Telnet_p = pcb;
    // printf("Telnet accept <0x%x>\r\n",pcb);
    DbgLog(DBG_TCP_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet accept <0x%x>\r\n", pcb);
    g_ucTelnetCmdState = TELNET_DOECHO_CMD;
    tcp_write(pcb, g_szTelnetDoEchoCmd, 3, TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);

    return ERR_OK;
}

void Telnet_Init(void)
{
    struct tcp_pcb *pcb;
    SysParams *pstSysParams = NULL;

    g_stGlobeOps.sys_param_ops.param_get(&pstSysParams);
    g_stGlobeOps.sem_ops.init("ping_sem", 0, &s_ping_sem);
    if (pstSysParams->NetParam.telnet_en != 0)
    {
        pcb = tcp_new();
        if (pcb != NULL)
        {
            if (pstSysParams->NetParam.ServerPort == 2333)
            {
                tcp_bind(pcb, IP_ADDR_ANY, 2334);
            }
            else
            {
                tcp_bind(pcb, IP_ADDR_ANY, 2333);
            }
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, telnet_accept);
        }
        DbgLog(DBG_TCP_SW, DBG_DETAIL, DBG_COLOR_YELLOW, DBG_TS_EN, "Telnet Listen Pcb<0x%x>\r\n", pcb);
    }

    icmp_pcb_init();
}
