/**
 * @file api_sys_param.c
 * @brief ϵͳ������ز���ʵ��
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
 * @brief ��ȡϵͳ������ֻ����
 * @param[out] pparam ϵͳ����ָ��
 * @retval 0 �ɹ�
 */
int Api_Sys_Param_Get(SysParams **pparam)
{
    *pparam = &s_stSysConf;
    return 0;
}

/**
 * @brief ��ȡϵͳ����Ӱ�ӽṹ�壨�����޸ģ�
 * @param[out] pparam Ӱ�Ӳ���ָ��
 * @retval 0 �ɹ�
 */
int Api_Sys_Param_Shadow_Get(SysParams **pparam)
{
    *pparam = &s_stSysConfShadow;
    return 0;
}

/**
 * @brief ��Flash�ָ�ϵͳ�������Զ�У������CRC���ָ�
 * @retval 0 �ɹ���-1 ʧ��
 */
int Api_Sys_Param_Recovery(void)
{
    // �������ݶ�д�ṹ���Flash�����ṹ��
    RevData stRevData;
    TransData stTransData;
    FlashParams stFlashParams;
    // CRCУ��ֵ
    unsigned short usCrc = 0, usCrcShadow = 0;
    int ret = -1;

    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);
    
    // ������������ȡϵͳ����
    stRevData.pllif_fd = s_ApiSysParams.sysconf_fd;
    stRevData.want_len = sizeof(SysParams);
    stRevData.pbuff = (unsigned char *)&s_stSysConf;
    stRevData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    g_stGlobeOps.if_ops.read(&stRevData);
    // ������������CRCУ��ֵ
    usCrc = GetCrc16(0, (unsigned char *)&s_stSysConf, sizeof(SysParams) - 2);

    if (s_stSysConf.usCrc == usCrc)
    {
        // ��������CRCУ��ɹ�
        ret = 0;
        // ��ȡ��������������
        stRevData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
        stRevData.want_len = sizeof(SysParams);
        stRevData.pbuff = (unsigned char *)&s_stSysConfShadow;
        stRevData.opt = &stFlashParams;
        stFlashParams.offset = 0;
        g_stGlobeOps.if_ops.read(&stRevData);
        // ���㱸��������CRCУ��ֵ
        usCrcShadow = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
        
        // ��鱸����CRC����������������һ����
        if ((usCrcShadow == s_stSysConfShadow.usCrc) && (s_stSysConfShadow.usCrc == s_stSysConf.usCrc))
        {
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_GREEN, DBG_TS_EN, "[Api Sys Param] System Config Get Success, Pair Success!\r\n");
        }
        else
        {
            // ������CRCУ��ʧ�ܻ�������һ�£�ʹ�������������ݻָ�������
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
        // ��������CRCУ��ʧ�ܣ����Դӱ������ָ�
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] System Config Sector Get Error! Try Bak Sector ...\r\n\r\n");
        stRevData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
        stRevData.want_len = sizeof(SysParams);
        stRevData.pbuff = (unsigned char *)&s_stSysConfShadow;
        stRevData.opt = &stFlashParams;
        stFlashParams.offset = 0;
        g_stGlobeOps.if_ops.read(&stRevData);
        // ���㱸����CRCУ��ֵ
        usCrcShadow = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
        if (usCrcShadow == s_stSysConfShadow.usCrc)
        {
            // ������CRCУ��ɹ���ʹ�ñ��������ݻָ���������
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
            // ������������У��ʧ��
            logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] System Config Shadow Sector Get Error!\r\n");
        }
    }
    return ret;
}

/**
 * @brief ����Ӱ�Ӳ�����Flash
 * @retval 0 �ɹ�
 */
int Api_Sys_Param_Save(void)
{
    // �������ݴ���ṹ���Flash�����ṹ��
    TransData stTransData;
    FlashParams stFlashParams;
    unsigned short usCrc = 0;

    // ��ӡ������־
    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);

    // ����Ӱ�Ӳ�����CRCУ��ֵ
    usCrc = GetCrc16(0, (unsigned char *)&s_stSysConfShadow, sizeof(SysParams) - 2);
    s_stSysConfShadow.usCrc = usCrc;

    // ��Ӱ�Ӳ������Ƶ��������ṹ��
    memcpy(&s_stSysConf, &s_stSysConfShadow, sizeof(SysParams));

    // ����д�����������Ĳ���
    stTransData.pllif_fd = s_ApiSysParams.sysconf_fd;
    stTransData.ilen = sizeof(SysParams);
    stTransData.pbuff = (unsigned char *)&s_stSysConf;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    stFlashParams.flg = FLASH_ERASER_ENABLE;

    // д����������
    g_stGlobeOps.if_ops.write(&stTransData);

    // �л���������������д����ͬ������
    stTransData.pllif_fd = s_ApiSysParams.sysconfbak_fd;
    g_stGlobeOps.if_ops.write(&stTransData);

    return 0;
}

/**
 * @brief �ָ�ϵͳ����ΪĬ��ֵ
 * @param[in] flag 0-ȫ��գ�1-���ָֻ�
 */
void Api_Sys_Param_Recovery_Default(int flag)
{
    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);
    if (flag == 0)
    {
        memset(&s_stSysConf, 0, sizeof(SysParams));
        // ��ʼ����������е�MAC��ַ
        s_stSysConf.NetParam.szMac[0] = 0x52;
        s_stSysConf.NetParam.szMac[1] = 0x3A;
        s_stSysConf.NetParam.szMac[2] = 0x80;
        s_stSysConf.NetParam.szMac[3] = 0x00;
        s_stSysConf.NetParam.szMac[4] = 0x00;
        s_stSysConf.NetParam.szMac[5] = 0x01;
    }
    // ��Ʒ��Ϣ
    memset(s_stSysConf.ProdectInfo.szProductName, 0, sizeof(s_stSysConf.ProdectInfo.szProductName));
    memset(s_stSysConf.ProdectInfo.szProductCode, 0, sizeof(s_stSysConf.ProdectInfo.szProductCode));
    memcpy(s_stSysConf.ProdectInfo.szProductName, "TEST_DEMO_00001", 15);
    memcpy(s_stSysConf.ProdectInfo.szProductCode, "P22041101220613", 15);
    memset(s_stSysConf.ProdectInfo.szRes, 0, sizeof(s_stSysConf.ProdectInfo.szRes));
    // �������
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
    s_stSysConf.NetParam.DeviceRole = 0; // DEVICE_SERVER������ֵ�����ʵ�ʺ궨��
    memset(s_stSysConf.NetParam.Dname, 0, sizeof(s_stSysConf.NetParam.Dname));
    s_stSysConf.NetParam.DnameEn = 0;
    memset(s_stSysConf.NetParam.szRes, 0, sizeof(s_stSysConf.NetParam.szRes));
    // �豸����
    memset(s_stSysConf.DeviceParam.szDevID, 0, sizeof(s_stSysConf.DeviceParam.szDevID));
    s_stSysConf.DeviceParam.wdt_test = 0;
    memset(s_stSysConf.DeviceParam.szRes, 0, sizeof(s_stSysConf.DeviceParam.szRes));
    // ������Ϣ
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
    // ���ڲ���
    s_stSysConf.SerialParam.dwBaudrate = 115200;
    s_stSysConf.SerialParam.ucSerialMod = 0;
    s_stSysConf.SerialParam.ucStopBits = 1;
    s_stSysConf.SerialParam.ucWordBits = 8;
    s_stSysConf.SerialParam.ucParityBits = 0;
    s_stSysConf.SerialParam.ucUartKind = 0;
    memset(s_stSysConf.SerialParam.szRes, 0, sizeof(s_stSysConf.SerialParam.szRes));
    // ���ṹ�屣���ֽ�����
    memset(s_stSysConf.szRes, 0, sizeof(s_stSysConf.szRes));
    // CRC ����
    s_stSysConf.usCrc = 0;
}

/**
 * @brief ϵͳ������ʼ��
 * @retval 0 �ɹ���-1 ʧ��
 */
int Api_Sys_Param_Init(void)
{
    // �����ڲ�Flash�����ṹ��
    llifParams params;
    FlashInitParams stFlashInitParams;

    logs(DBG_INTERFLASH_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Sys Param] Enter %s\n", __FUNCTION__);

    // �������������洢λ��
    stFlashInitParams.pos = STORE_SYSTEMPARAM;

    // ����Flash�ӿڲ���
    params.inf_type = INF_INTERFLASH;
    params.params = &stFlashInitParams;

    // ����������Flash
    if (-1 == g_stGlobeOps.if_ops.open(&params, &s_ApiSysParams.sysconf_fd))
    {
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] Inter Flash Open error!\n");
        return -1;
    }

    // ���ñ����������洢λ��
    stFlashInitParams.pos = STORE_SYSTEMBAKPARAM;

    // �򿪱���������Flash
    if (-1 == g_stGlobeOps.if_ops.open(&params, &s_ApiSysParams.sysconfbak_fd))
    {
        logs(DBG_INTERFLASH_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Sys Param] Inter Flash Bak Open error!\n");
        return -1;
    }

    // ע��ϵͳ������������
    g_stGlobeOps.sys_param_ops.param_get = Api_Sys_Param_Get;
    g_stGlobeOps.sys_param_ops.shadow_param_get = Api_Sys_Param_Shadow_Get;
    g_stGlobeOps.sys_param_ops.save_to_flash = Api_Sys_Param_Save;
    g_stGlobeOps.sys_param_ops.recovery_from_flash = Api_Sys_Param_Recovery;
    g_stGlobeOps.sys_param_ops.recovery_default_setting = Api_Sys_Param_Recovery_Default;

    // ��Flash�ָ�ϵͳ���������ʧ����ָ�Ĭ��ֵ������
    if (0 != g_stGlobeOps.sys_param_ops.recovery_from_flash())
    {
        g_stGlobeOps.sys_param_ops.recovery_default_setting(SYSPARAM_RECOVERY_FIRST);
        g_stGlobeOps.sys_param_ops.save_to_flash();
    }

    return 0;
}
API_INIT(Api_Sys_Param_Init);
