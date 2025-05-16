/**
 * @file api_sys_param.c
 * @brief 系统参数相关操作实现
 * @author 
 * @date 
 */
#include <stdio.h>
#include <string.h>
#include "includes.h"
#include "main.h"
#include "common.h"
#include "interface.h"
#include "api_sys_param.h"

static ApiSysParams s_ApiSysParams;
static SysParams s_stSysConf, s_stSysConfShadow;

/**
 * @brief 获取系统参数（只读）
 * @param[out] pparam 系统参数指针
 * @retval 0 成功
 */
int Api_Sys_Param_Get(SysParams **pparam)
{
    *pparam = &s_stSysConf;
    return 0;
}

/**
 * @brief 获取系统参数影子结构体（用于修改）
 * @param[out] pparam 影子参数指针
 * @retval 0 成功
 */
int Api_Sys_Param_Shadow_Get(SysParams **pparam)
{
    *pparam = &s_stSysConfShadow;
    return 0;
}

/**
 * @brief 从Flash恢复系统参数，自动校验主备CRC并恢复
 * @retval 0 成功，-1 失败
 */
int Api_Sys_Param_Recovery(void)
{
    // 定义数据读写结构体和Flash参数结构体
    RevData stRevData;
    TransData stTransData;
    FlashParams stFlashParams;
    // CRC校验值
    unsigned short usCrc = 0, usCrcShadow = 0;
    int ret = -1;

    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);
    
    // 从主配置区读取系统参数
    stRevData.pllif_fd = s_ApiSysParams.sysconf_fd;
    stRevData.want_len = sizeof(SysParams);
    stRevData.pbuff = (unsigned char *)&s_stSysConf;
    stRevData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    g_stGlobeOps.if_ops.read(&stRevData);
    // 计算主配置区CRC校验值
    usCrc = GetCrc16(0, (unsigned char *)&s_stSysConf, sizeof(SysParams) - 2);

    if (s_stSysConf.usCrc == usCrc)
    {
        // 主配置区CRC校验成功
        ret = 0;
        // 读取备份配置区数据
        stRevData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
        stRevData.want_len = sizeof(SysParams);
        stRevData.pbuff = (unsigned char *)&s_stSysConfShadow;
        stRevData.opt = &stFlashParams;
        stFlashParams.offset = 0;
        g_stGlobeOps.if_ops.read(&stRevData);
        // 计算备份配置区CRC校验值
        usCrcShadow = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
        
        // 检查备份区CRC和主备配置区数据一致性
        if ((usCrcShadow == s_stSysConfShadow.usCrc) && (s_stSysConfShadow.usCrc == s_stSysConf.usCrc))
        {
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_GREEN, DBG_TS_EN, "[Api Sys Param] System Config Get Success, Pair Success!\r\n");
        }
        else
        {
            // 备份区CRC校验失败或主备不一致，使用主配置区数据恢复备份区
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] System Config Shadow Sector Get Fail or Pair Fail! Recovery Config Bak Sector!\r\n");
            stTransData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
            stTransData.ilen = sizeof(SysParams);
            stTransData.pbuff = (unsigned char *)&s_stSysConf;
            stTransData.opt = &stFlashParams;
            stFlashParams.offset = 0;
            stFlashParams.flg = FLASH_ERASER_ENABLE;
            g_stGlobeOps.if_ops.write(&stTransData);
        }
    }
    else
    {
        // 主配置区CRC校验失败，尝试从备份区恢复
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] System Config Sector Get Error! Try Bak Sector ...\r\n\r\n");
        stRevData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
        stRevData.want_len = sizeof(SysParams);
        stRevData.pbuff = (unsigned char *)&s_stSysConfShadow;
        stRevData.opt = &stFlashParams;
        stFlashParams.offset = 0;
        g_stGlobeOps.if_ops.read(&stRevData);
        // 计算备份区CRC校验值
        usCrcShadow = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
        if (usCrcShadow == s_stSysConfShadow.usCrc)
        {
            // 备份区CRC校验成功，使用备份区数据恢复主配置区
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_GREEN, DBG_TS_EN, "[Api Sys Param] System Config Shadow Sector Get Succ, Recovery Config Sector!\r\n");
            ret = 0;
            stTransData.pllif_fd = s_ApiSysParams.sysconf_fd;
            stTransData.ilen = sizeof(SysParams);
            stTransData.pbuff = (unsigned char *)&s_stSysConfShadow;
            stTransData.opt = &stFlashParams;
            stFlashParams.offset = 0;
            stFlashParams.flg = FLASH_ERASER_ENABLE;
            g_stGlobeOps.if_ops.write(&stTransData);
        }
        else
        {
            // 主备配置区均校验失败
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] System Config Shadow Sector Get Error!\r\n");
        }
    }
    return ret;
}

/**
 * @brief 保存影子参数到Flash
 * @retval 0 成功
 */
int Api_Sys_Param_Save(void)
{
    // 定义数据传输结构体和Flash参数结构体
    TransData stTransData;
    FlashParams stFlashParams;
    unsigned short usCrc = 0;

    // 打印调试日志
    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);

    // 计算影子参数的CRC校验值
    usCrc = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
    s_stSysConfShadow.usCrc = usCrc;

    // 将影子参数复制到主参数结构体
    memcpy(&s_stSysConf, &s_stSysConfShadow, sizeof(SysParams));

    // 配置写入主配置区的参数
    stTransData.pllif_fd = s_ApiSysParams.sysconf_fd;
    stTransData.ilen = sizeof(SysParams);
    stTransData.pbuff = (unsigned char *)&s_stSysConf;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    stFlashParams.flg = FLASH_ERASER_ENABLE;

    // 写入主配置区
    g_stGlobeOps.if_ops.write(&stTransData);

    // 切换到备份配置区并写入相同的数据
    stTransData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
    g_stGlobeOps.if_ops.write(&stTransData);

    return 0;
}

/**
 * @brief 恢复系统参数为默认值
 * @param[in] flag 0-全清空，1-部分恢复
 */
void Api_Sys_Param_Recovery_Default(int flag)
{
    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);
    if (flag == 0)
    {
        memset(&s_stSysConf, 0, sizeof(SysParams));
        // 初始化网络参数中的MAC地址
        s_stSysConf.NetParam.szMac[0] = 0x52;
        s_stSysConf.NetParam.szMac[1] = 0x3A;
        s_stSysConf.NetParam.szMac[2] = 0x80;
        s_stSysConf.NetParam.szMac[3] = 0x00;
        s_stSysConf.NetParam.szMac[4] = 0x00;
        s_stSysConf.NetParam.szMac[5] = 0x01;
    }
    // 产品信息
    memset(s_stSysConf.ProdectInfo.szProductName, 0, sizeof(s_stSysConf.ProdectInfo.szProductName));
    memset(s_stSysConf.ProdectInfo.szProductCode, 0, sizeof(s_stSysConf.ProdectInfo.szProductCode));
    memcpy(s_stSysConf.ProdectInfo.szProductName, "TEST_DEMO_00001", 15);
    memcpy(s_stSysConf.ProdectInfo.szProductCode, "P22041101220613", 15);
    memset(s_stSysConf.ProdectInfo.szRes, 0, sizeof(s_stSysConf.ProdectInfo.szRes));
    // 网络参数
    s_stSysConf.NetParam.LocalIp[0] = 192;
    s_stSysConf.NetParam.LocalIp[1] = 168;
    s_stSysConf.NetParam.LocalIp[2] = 1;
    s_stSysConf.NetParam.LocalIp[3] = 100;
    s_stSysConf.NetParam.LocalPort = 7880;
    s_stSysConf.NetParam.Mask[0] = 255;
    s_stSysConf.NetParam.Mask[1] = 255;
    s_stSysConf.NetParam.Mask[2] = 255;
    s_stSysConf.NetParam.Mask[3] = 0;
    s_stSysConf.NetParam.gateway[0] = 192;
    s_stSysConf.NetParam.gateway[1] = 168;
    s_stSysConf.NetParam.gateway[2] = 1;
    s_stSysConf.NetParam.gateway[3] = 20;
    s_stSysConf.NetParam.ServerIp[0] = 192;
    s_stSysConf.NetParam.ServerIp[1] = 168;
    s_stSysConf.NetParam.ServerIp[2] = 1;
    s_stSysConf.NetParam.ServerIp[3] = 99;
    s_stSysConf.NetParam.ServerPort = 7880;
    s_stSysConf.NetParam.telnet_en = 0;
    s_stSysConf.NetParam.DeviceRole = 0; // DEVICE_SERVER，具体值请根据实际宏定义
    memset(s_stSysConf.NetParam.Dname, 0, sizeof(s_stSysConf.NetParam.Dname));
    s_stSysConf.NetParam.DnameEn = 0;
    memset(s_stSysConf.NetParam.szRes, 0, sizeof(s_stSysConf.NetParam.szRes));
    // 设备参数
    memset(s_stSysConf.DeviceParam.szDevID, 0, sizeof(s_stSysConf.DeviceParam.szDevID));
    s_stSysConf.DeviceParam.wdt_test = 0;
    memset(s_stSysConf.DeviceParam.szRes, 0, sizeof(s_stSysConf.DeviceParam.szRes));
    // 升级信息
    s_stSysConf.UpdateInfo.valid = 0;
    memset(s_stSysConf.UpdateInfo.uuid, 0, sizeof(s_stSysConf.UpdateInfo.uuid));
    memset(s_stSysConf.UpdateInfo.ver, 0, sizeof(s_stSysConf.UpdateInfo.ver));
    memset(s_stSysConf.UpdateInfo.url, 0, sizeof(s_stSysConf.UpdateInfo.url));
    s_stSysConf.UpdateInfo.size = 0;
    s_stSysConf.UpdateInfo.index = 0;
    s_stSysConf.UpdateInfo.NeedReport = 0;
    s_stSysConf.UpdateInfo.HadCrc = 0;
    s_stSysConf.UpdateInfo.crc = 0;
    s_stSysConf.UpdateInfo.UpdateMethod = 0;
    s_stSysConf.UpdateInfo.FirmwareKind = 0;
    memset(s_stSysConf.UpdateInfo.szRes, 0, sizeof(s_stSysConf.UpdateInfo.szRes));
    // 串口参数
    s_stSysConf.SerialParam.dwBaudrate = 115200;
    s_stSysConf.SerialParam.ucSerialMod = 0;
    s_stSysConf.SerialParam.ucStopBits = 1;
    s_stSysConf.SerialParam.ucWordBits = 8;
    s_stSysConf.SerialParam.ucParityBits = 0;
    s_stSysConf.SerialParam.ucUartKind = 0;
    memset(s_stSysConf.SerialParam.szRes, 0, sizeof(s_stSysConf.SerialParam.szRes));
    // 主结构体保留字节清零
    memset(s_stSysConf.szRes, 0, sizeof(s_stSysConf.szRes));
    // CRC 置零
    s_stSysConf.usCrc = 0;
}

/**
 * @brief 系统参数初始化
 * @retval 0 成功，-1 失败
 */
int Api_Sys_Param_Init(void)
{
    // 定义内部Flash参数结构体
    llifParams params;
    FlashInitParams stFlashInitParams;

    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);

    // 设置主配置区存储位置
    stFlashInitParams.pos = STORE_SYSTEMPARAM;

    // 配置Flash接口参数
    params.inf_type = INF_INTERFLASH;
    params.params = &stFlashInitParams;

    // 打开主配置区Flash
    if (-1 == g_stGlobeOps.if_ops.open(&params, &s_ApiSysParams.sysconf_fd))
    {
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] Inter Flash Open error!\n");
        return -1;
    }

    // 设置备份配置区存储位置
    stFlashInitParams.pos = STORE_SYSTEMBAKPARAM;

    // 打开备份配置区Flash
    if (-1 == g_stGlobeOps.if_ops.open(&params, &s_ApiSysParams.sysconfbak_fd))
    {
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] Inter Flash Bak Open error!\n");
        return -1;
    }

    // 注册系统参数操作函数
    g_stGlobeOps.sys_param_ops.param_get = Api_Sys_Param_Get;
    g_stGlobeOps.sys_param_ops.shadow_param_get = Api_Sys_Param_Shadow_Get;
    g_stGlobeOps.sys_param_ops.save_to_flash = Api_Sys_Param_Save;
    g_stGlobeOps.sys_param_ops.recovery_from_flash = Api_Sys_Param_Recovery;
    g_stGlobeOps.sys_param_ops.recovery_default_setting = Api_Sys_Param_Recovery_Default;

    // 从Flash恢复系统参数，如果失败则恢复默认值并保存
    if (0 != g_stGlobeOps.sys_param_ops.recovery_from_flash())
    {
        g_stGlobeOps.sys_param_ops.recovery_default_setting(SYSPARAM_RECOVERY_FIRST);
        g_stGlobeOps.sys_param_ops.save_to_flash();
    }

    return 0;
}
API_INIT(Api_Sys_Param_Init);
