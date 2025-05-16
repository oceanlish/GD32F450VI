/**
 * @file api_sys_param.h
 * @brief ϵͳ������ؽṹ��������ӿڶ���
 * @author 
 * @date 
 */

#ifndef _API_SYS_PARAM_H_
#define _API_SYS_PARAM_H_

#include "inf_defs.h"

/** ϵͳ�����ָ����Ͷ��� */
#define SYSPARAM_RECOVERY_FIRST 0    /**< �״λָ� */
#define SYSPARAM_RECOVERY_FACTORY 1   /**< �ָ��������� */

/** ͨ�Žӿ����Ͷ��� */
#define UART_INTERFACE 0     /**< ���ڽӿ� */
#define NET_INTERFACE 1      /**< ����ӿ� */

/** ������ʽ���� */
#define EDGE_TRIGGER 1       /**< ���ش��� */
#define LEVEL_TRIGGER 0      /**< ��ƽ���� */

#pragma pack(1)

/**
 * @brief �豸�����ṹ�� 152
 */
typedef struct
{
    unsigned char szDevID[4];        /**< �豸ID */
    unsigned char wdt_test;          /**< ���Ź�����ʹ�ܱ�־ */
    unsigned char szRes[147];        /**< �����ֽ� */
} ST_DEVICE_PARAM;

/**
 * @brief ��������ṹ�� 184
 */
typedef struct
{
    unsigned char LocalIp[4];        /**< ����IP��ַ */
    unsigned char Mask[4];           /**< �������� */
    unsigned char gateway[4];        /**< ���ص�ַ */
    unsigned char ServerIp[4];       /**< ������IP��ַ */
    unsigned short ServerPort;       /**< �������˿ں� */
    unsigned short LocalPort;        /**< ���ض˿ں� */
    unsigned char szMac[6];          /**< MAC��ַ */
    char Dname[99];                  /**< ���� */
    char DnameEn;                    /**< ����ʹ�ܱ�־ */
    unsigned char telnet_en;         /**< telnetʹ�ܱ�־ */
    unsigned char DeviceRole;        /**< �豸��ɫ */
    unsigned char szRes[56];         /**< �����ֽ� */
} ST_NET_PARAM;

/**
 * @brief ��Ʒ��Ϣ�ṹ�� 120
 */
typedef struct
{
    char szProductName[50];          /**< ��Ʒ���� */
    char szProductCode[50];          /**< ��Ʒ���� */
    unsigned char szRes[20];         /**< �����ֽ� */
} ST_PRODUCT_INFO;

/**
 * @brief ������Ϣ�ṹ�� 240
 */
typedef struct
{
    int valid;                     /**< ������Ϣ��Ч��־ */
    char uuid[50];                 /**< ������Ψһ��ʶ�� */
    char ver[50];                  /**< �������汾�� */
    char url[100];                 /**< ���������ص�ַ */
    unsigned int size;             /**< ��������С */
    unsigned short index;          /**< ��������� */
    char NeedReport;               /**< �Ƿ���Ҫ�ϱ�������� */
    char HadCrc;                   /**< �Ƿ��ѽ���CRCУ�� */
    unsigned int crc;              /**< CRCУ��ֵ */
    char UpdateMethod;             /**< ������ʽ */
    unsigned char FirmwareKind;    /**< �̼����� */
    unsigned char szRes[22];       /**< �����ֽ� */
} ST_UPDATE_INFO;

/**
 * @brief ���ڲ����ṹ�� 16
 */
typedef struct
{
    unsigned int dwBaudrate;        /**< ���ڲ����� */
    unsigned char ucSerialMod;      /**< ���ڹ���ģʽ */
    unsigned char ucStopBits;       /**< ֹͣλ���� */
    unsigned char ucWordBits;       /**< ����λ���� */
    unsigned char ucParityBits;     /**< У��λ���� */
    unsigned char ucUartKind;       /**< �������� */
    unsigned char szRes[7];         /**< �����ֽ� */
} ST_SERIAL_INFO;


/**
 * @brief ϵͳ�������ṹ�� 2048
 */
typedef struct
{
    ST_DEVICE_PARAM DeviceParam;     /**< �豸���� */
    ST_NET_PARAM NetParam;           /**< ������� */
    ST_PRODUCT_INFO ProdectInfo;     /**< ��Ʒ��Ϣ */
    ST_UPDATE_INFO UpdateInfo;       /**< ������Ϣ */
    ST_SERIAL_INFO SerialParam;      /**< ���ڲ��� */

    unsigned char szRes[1334];       /**< �����ֽ� */
    unsigned short usCrc;            /**< CRCУ��ֵ */
} SysParams;

#pragma pack()

/**
 * @brief ϵͳ���������ӿ�
 */
typedef struct
{
    /** ��ȡϵͳ����ָ�� */
    int (*param_get)(SysParams **);
    
    /** ��ȡϵͳ����Ӱ�ӿ���ָ�� */
    int (*shadow_param_get)(SysParams **);
    
    /** ��ϵͳ�������浽Flash */
    int (*save_to_flash)(void);
    
    /** ��Flash�ָ�ϵͳ���� */
    int (*recovery_from_flash)(void);
    
    /** �ָ�Ĭ��ϵͳ��������
     * @param type �ָ�����:
     *            SYSPARAM_RECOVERY_FIRST - �״λָ�
     *            SYSPARAM_RECOVERY_FACTORY - �ָ���������
     */
    void (*recovery_default_setting)(int);
} ApiSysParamOps;

/**
 * @brief ϵͳ�����ļ��������ṹ��
 */
typedef struct
{
    void *sysconf_fd;
    void *sysconfbak_fd;
} ApiSysParams;

#endif
