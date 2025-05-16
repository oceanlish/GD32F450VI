/**
 * @file api_upgrade.c
 * @brief �������Flash�����ӿ�ʵ�֡�
 */

#include "main.h"
#include "includes.h"
#include "api_upgrade.h"

// ȫ�ֱ���������Flash�ļ�������
static void *g_upgrade_fd;
// Flashд���ַָ��
static __IO uint32_t RFAMFlashWriteAddress;
// ��4�ֽڶ���ʱ����ʣ���ֽ�
static char LeftBytesTab[4];
// ʣ���ֽ���
static uint8_t LeftBytes = 0;

/**
 * @brief ��ʼ�������õ�Flash�ӿڡ�
 * @return 0��ʾ�ɹ���-1��ʾʧ�ܡ�
 */
int Upgrade_Flash_If_Init(void)
{
    llifParams params;
    FlashInitParams stFlashInitParams;
    // ���ñ�����λ��
    stFlashInitParams.pos = STORE_APP_BAK;
    // ���ýӿ����ͺͲ���
    params.inf_type = INF_INTERFLASH;
    params.params = &stFlashInitParams;
    // ��Flash�ӿڣ�ʧ�ܷ���-1
    if (-1 == g_stGlobeOps.if_ops.open(&params, &g_upgrade_fd))
    {
        logs(DBG_HTTP_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Upgrade] Inter Flash Open error!\n");
        return -1;
    }
    return 0;
}
MODULE_INIT(Upgrade_Flash_If_Init);

/**
 * @brief ���������õ�Flash����
 * @note ������дָ���ʣ���ֽڻ���ᱻ���á�
 */
void Upgrade_Flash_If_Erase(void)
{
    TransData stTransData;
    FlashParams stFlashParams;
    // ���ò�������
    stTransData.pllif_fd = g_upgrade_fd;
    stTransData.ilen = 0;
    stTransData.pbuff = NULL;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    stFlashParams.flg = FLASH_ERASER_ENABLE;
    // ����������дָ��ͻ���
    RFAMFlashWriteAddress = 0;
    LeftBytes = 0;
    memset(LeftBytesTab, 0, sizeof(LeftBytesTab));
    // ִ�в�������
    g_stGlobeOps.if_ops.write(&stTransData);
}

/**
 * @brief ������Flashд�����ݡ�
 * @param pdata ����ָ�룬ָ���д������ݣ���uint32_tΪ��λ����
 * @param len   д������ݳ��ȣ���uint32_tΪ��λ����
 * @return д��ɹ�����д���ֽ�����ʧ�ܷ���-1��
 */
int Upgrade_Flash_If_Write(uint32_t *pdata, int len)
{
    TransData stTransData;
    FlashParams stFlashParams;
    // ����д�����
    stTransData.pllif_fd = g_upgrade_fd;
    stTransData.ilen = len * sizeof(uint32_t);
    stTransData.pbuff = (unsigned char *)pdata;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = RFAMFlashWriteAddress;
    stFlashParams.flg = 0;
    // ����дָ��
    RFAMFlashWriteAddress += stTransData.ilen;
    // д������
    return g_stGlobeOps.if_ops.write(&stTransData);
}

/**
 * @brief ������Flash��ȡ���ݡ�
 * @param offset ��ȡƫ�Ƶ�ַ��
 * @param pdata  ��ȡ���ݴ�ŵĻ�������
 * @param len    ��ȡ���ȣ��ֽڣ���
 * @return ��ȡ�ɹ����ض�ȡ�ֽ�����ʧ�ܷ���-1��
 */
int Upgrade_Flash_If_Read(volatile unsigned int offset, unsigned char *pdata, int len)
{
    RevData stRevData = {0};
    FlashParams stFlashParams;
    // ���ö�ȡ����
    stRevData.pllif_fd = g_upgrade_fd;
    stRevData.want_len = len;
    stRevData.pbuff = pdata;
    stRevData.opt = &stFlashParams;
    stFlashParams.offset = offset;
    stFlashParams.flg = 0;
    // ��ȡ����
    return g_stGlobeOps.if_ops.read(&stRevData);
}

/**
 * @brief ����д���������ݣ��Զ������4�ֽڶ��������
 * @param ptr           ����ָ�롣
 * @param len           ����д������ݳ��ȣ��ֽڣ���
 * @param TotalReceived ��ǰ�ѽ������ֽ�����
 * @param size          �����ݳ��ȣ��ֽڣ���
 * @return 0��ʾ�ɹ���-1��ʾʧ�ܡ�
 */
int Upgrade_Flash_writedata(char *ptr, uint32_t len, uint32_t TotalReceived, uint32_t size)
{
    uint32_t count, i = 0, j = 0;
    // ����Ƿ����ϴ�������δ�����ֽ�
    if (LeftBytes)
    {
        while (LeftBytes <= 3)
        {
            if (len > j)
            {
                LeftBytesTab[LeftBytes++] = *(ptr + j);
            }
            else
            {
                LeftBytesTab[LeftBytes++] = 0xFF; // ���㲹0xFF
            }
            j++;
        }
        // �����д��Flash
        if (Upgrade_Flash_If_Write((uint32_t *)(LeftBytesTab), 1) == -1)
        {
            return -1;
        }
        LeftBytes = 0;
        if (len > j)
        {
            // ����ָ��ͳ���
            ptr = (char *)(ptr + j);
            len = len - j;
        }
    }
    // ���������4�ֽڵ����ݿ���
    count = len / 4;
    // ���ʣ���ֽ�
    i = len % 4;
    if (i > 0)
    {
        if (TotalReceived != size)
        {
            // �����һ��������ʣ���ֽ�
            LeftBytes = 0;
            for (; i > 0; i--)
                LeftBytesTab[LeftBytes++] = *(char *)(ptr + len - i);
        }
        else
        {
            // ���һ��������д��
            count++;
        }
    }
    // д����������
    if (count > 0)
    {
        if (Upgrade_Flash_If_Write((uint32_t *)ptr, count) == -1)
        {
            return -1;
        }
    }
    return 0;
}
