#ifndef _SYS_BASE_H_
#define _SYS_BASE_H_

/**
 * @brief ϵͳָʾ�����ṹ��
 */
typedef struct
{
    void *pbeep_fd;        /**< �������ļ�������ָ�� */
    void *pbtn_fd;         /**< �����ļ�������ָ�� */
    void *pwdt_fd;         /**< ���Ź��ļ�������ָ�� */
    void *pmodule_rst_fd;  /**< ģ�鸴λ�ļ�������ָ�� */
} SysIndicationParams;

/**
 * @brief ��ʼ��LWIPЭ��ջ
 */
void Lwip_Init(void);

/**
 * @brief ������������
 * @param pvParameters �������
 */
void Net_Periodic_Task(void *pvParameters);

/**
 * @brief ϵͳ��ջ�������
 * @param pvParameters �������
 */
void Sys_Check_Stk_Task(void *pvParameters);

/**
 * @brief ϵͳ״̬����
 * @param pvParameters �������
 */
void Sys_State_Task(void *pvParameters);

/**
 * @brief ϵͳ��ʱ����
 * @param usSec ��ʱʱ�䣨�룩
 */
void Sys_Delay_Reset(unsigned short usSec);

/**
 * @brief ��ȡ�������ļ�������
 * @return �ļ�������ָ��
 */
void *Sys_GetBeepFd(void);

/**
 * @brief ��ȡLED�ļ�������
 * @return �ļ�������ָ��
 */
void *Sys_GetLedFd(void);

/**
 * @brief ��ֹι������
 */
void Sys_DisFeedWdtTest(void);

/**
 * @brief ��ֹι��
 */
void Sys_DisFeedWdt(void);

#endif

