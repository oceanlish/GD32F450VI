/**
 * @file api_sys_param.h
 * @brief 系统参数相关结构体与操作接口定义
 * @author 
 * @date 
 */

#ifndef _API_SYS_PARAM_H_
#define _API_SYS_PARAM_H_

#include "inf_defs.h"

/** 系统参数恢复类型定义 */
#define SYSPARAM_RECOVERY_FIRST 0    /**< 首次恢复 */
#define SYSPARAM_RECOVERY_FACTORY 1   /**< 恢复出厂设置 */

/** 通信接口类型定义 */
#define UART_INTERFACE 0     /**< 串口接口 */
#define NET_INTERFACE 1      /**< 网络接口 */

/** 触发方式定义 */
#define EDGE_TRIGGER 1       /**< 边沿触发 */
#define LEVEL_TRIGGER 0      /**< 电平触发 */

#pragma pack(1)

/**
 * @brief 设备参数结构体 152
 */
typedef struct
{
    unsigned char szDevID[4];        /**< 设备ID */
    unsigned char wdt_test;          /**< 看门狗测试使能标志 */
    unsigned char szRes[147];        /**< 保留字节 */
} ST_DEVICE_PARAM;

/**
 * @brief 网络参数结构体 184
 */
typedef struct
{
    unsigned char LocalIp[4];        /**< 本地IP地址 */
    unsigned char Mask[4];           /**< 子网掩码 */
    unsigned char gateway[4];        /**< 网关地址 */
    unsigned char ServerIp[4];       /**< 服务器IP地址 */
    unsigned short ServerPort;       /**< 服务器端口号 */
    unsigned short LocalPort;        /**< 本地端口号 */
    unsigned char szMac[6];          /**< MAC地址 */
    char Dname[99];                  /**< 域名 */
    char DnameEn;                    /**< 域名使能标志 */
    unsigned char telnet_en;         /**< telnet使能标志 */
    unsigned char DeviceRole;        /**< 设备角色 */
    unsigned char szRes[56];         /**< 保留字节 */
} ST_NET_PARAM;

/**
 * @brief 产品信息结构体 120
 */
typedef struct
{
    char szProductName[50];          /**< 产品名称 */
    char szProductCode[50];          /**< 产品编码 */
    unsigned char szRes[20];         /**< 保留字节 */
} ST_PRODUCT_INFO;

/**
 * @brief 升级信息结构体 240
 */
typedef struct
{
    int valid;                     /**< 升级信息有效标志 */
    char uuid[50];                 /**< 升级包唯一标识符 */
    char ver[50];                  /**< 升级包版本号 */
    char url[100];                 /**< 升级包下载地址 */
    unsigned int size;             /**< 升级包大小 */
    unsigned short index;          /**< 升级包序号 */
    char NeedReport;               /**< 是否需要上报升级结果 */
    char HadCrc;                   /**< 是否已进行CRC校验 */
    unsigned int crc;              /**< CRC校验值 */
    char UpdateMethod;             /**< 升级方式 */
    unsigned char FirmwareKind;    /**< 固件类型 */
    unsigned char szRes[22];       /**< 保留字节 */
} ST_UPDATE_INFO;

/**
 * @brief 串口参数结构体 16
 */
typedef struct
{
    unsigned int dwBaudrate;        /**< 串口波特率 */
    unsigned char ucSerialMod;      /**< 串口工作模式 */
    unsigned char ucStopBits;       /**< 停止位设置 */
    unsigned char ucWordBits;       /**< 数据位长度 */
    unsigned char ucParityBits;     /**< 校验位设置 */
    unsigned char ucUartKind;       /**< 串口类型 */
    unsigned char szRes[7];         /**< 保留字节 */
} ST_SERIAL_INFO;


/**
 * @brief 系统参数主结构体 2048
 */
typedef struct
{
    ST_DEVICE_PARAM DeviceParam;     /**< 设备参数 */
    ST_NET_PARAM NetParam;           /**< 网络参数 */
    ST_PRODUCT_INFO ProdectInfo;     /**< 产品信息 */
    ST_UPDATE_INFO UpdateInfo;       /**< 升级信息 */
    ST_SERIAL_INFO SerialParam;      /**< 串口参数 */

    unsigned char szRes[1334];       /**< 保留字节 */
    unsigned short usCrc;            /**< CRC校验值 */
} SysParams;

#pragma pack()

/**
 * @brief 系统参数操作接口
 */
typedef struct
{
    /** 获取系统参数指针 */
    int (*param_get)(SysParams **);
    
    /** 获取系统参数影子拷贝指针 */
    int (*shadow_param_get)(SysParams **);
    
    /** 将系统参数保存到Flash */
    int (*save_to_flash)(void);
    
    /** 从Flash恢复系统参数 */
    int (*recovery_from_flash)(void);
    
    /** 恢复默认系统参数设置
     * @param type 恢复类型:
     *            SYSPARAM_RECOVERY_FIRST - 首次恢复
     *            SYSPARAM_RECOVERY_FACTORY - 恢复出厂设置
     */
    void (*recovery_default_setting)(int);
} ApiSysParamOps;

/**
 * @brief 系统参数文件描述符结构体
 */
typedef struct
{
    void *sysconf_fd;
    void *sysconfbak_fd;
} ApiSysParams;

#endif
