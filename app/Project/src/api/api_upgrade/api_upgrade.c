/**
 * @file api_upgrade.c
 * @brief 升级相关Flash操作接口实现。
 */

#include "main.h"
#include "includes.h"
#include "api_upgrade.h"

// 全局变量：升级Flash文件描述符
static void *g_upgrade_fd;
// Flash写入地址指针
static __IO uint32_t RFAMFlashWriteAddress;
// 非4字节对齐时缓存剩余字节
static char LeftBytesTab[4];
// 剩余字节数
static uint8_t LeftBytes = 0;

/**
 * @brief 初始化升级用的Flash接口。
 * @return 0表示成功，-1表示失败。
 */
int Upgrade_Flash_If_Init(void)
{
    llifParams params;
    FlashInitParams stFlashInitParams;
    // 设置备份区位置
    stFlashInitParams.pos = STORE_APP_BAK;
    // 配置接口类型和参数
    params.inf_type = INF_INTERFLASH;
    params.params = &stFlashInitParams;
    // 打开Flash接口，失败返回-1
    if (-1 == g_stGlobeOps.if_ops.open(&params, &g_upgrade_fd))
    {
        logs(DBG_HTTP_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Upgrade] Inter Flash Open error!\n");
        return -1;
    }
    return 0;
}
MODULE_INIT(Upgrade_Flash_If_Init);

/**
 * @brief 擦除升级用的Flash区域。
 * @note 擦除后写指针和剩余字节缓存会被重置。
 */
void Upgrade_Flash_If_Erase(void)
{
    TransData stTransData;
    FlashParams stFlashParams;
    // 配置擦除参数
    stTransData.pllif_fd = g_upgrade_fd;
    stTransData.ilen = 0;
    stTransData.pbuff = NULL;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = 0;
    stFlashParams.flg = FLASH_ERASER_ENABLE;
    // 擦除后重置写指针和缓存
    RFAMFlashWriteAddress = 0;
    LeftBytes = 0;
    memset(LeftBytesTab, 0, sizeof(LeftBytesTab));
    // 执行擦除操作
    g_stGlobeOps.if_ops.write(&stTransData);
}

/**
 * @brief 向升级Flash写入数据。
 * @param pdata 数据指针，指向待写入的数据（以uint32_t为单位）。
 * @param len   写入的数据长度（以uint32_t为单位）。
 * @return 写入成功返回写入字节数，失败返回-1。
 */
int Upgrade_Flash_If_Write(uint32_t *pdata, int len)
{
    TransData stTransData;
    FlashParams stFlashParams;
    // 配置写入参数
    stTransData.pllif_fd = g_upgrade_fd;
    stTransData.ilen = len * sizeof(uint32_t);
    stTransData.pbuff = (unsigned char *)pdata;
    stTransData.opt = &stFlashParams;
    stFlashParams.offset = RFAMFlashWriteAddress;
    stFlashParams.flg = 0;
    // 更新写指针
    RFAMFlashWriteAddress += stTransData.ilen;
    // 写入数据
    return g_stGlobeOps.if_ops.write(&stTransData);
}

/**
 * @brief 从升级Flash读取数据。
 * @param offset 读取偏移地址。
 * @param pdata  读取数据存放的缓冲区。
 * @param len    读取长度（字节）。
 * @return 读取成功返回读取字节数，失败返回-1。
 */
int Upgrade_Flash_If_Read(volatile unsigned int offset, unsigned char *pdata, int len)
{
    RevData stRevData = {0};
    FlashParams stFlashParams;
    // 配置读取参数
    stRevData.pllif_fd = g_upgrade_fd;
    stRevData.want_len = len;
    stRevData.pbuff = pdata;
    stRevData.opt = &stFlashParams;
    stFlashParams.offset = offset;
    stFlashParams.flg = 0;
    // 读取数据
    return g_stGlobeOps.if_ops.read(&stRevData);
}

/**
 * @brief 按需写入升级数据，自动处理非4字节对齐情况。
 * @param ptr           数据指针。
 * @param len           本次写入的数据长度（字节）。
 * @param TotalReceived 当前已接收总字节数。
 * @param size          总数据长度（字节）。
 * @return 0表示成功，-1表示失败。
 */
int Upgrade_Flash_writedata(char *ptr, uint32_t len, uint32_t TotalReceived, uint32_t size)
{
    uint32_t count, i = 0, j = 0;
    // 检查是否有上次遗留的未对齐字节
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
                LeftBytesTab[LeftBytes++] = 0xFF; // 不足补0xFF
            }
            j++;
        }
        // 补齐后写入Flash
        if (Upgrade_Flash_If_Write((uint32_t *)(LeftBytesTab), 1) == -1)
        {
            return -1;
        }
        LeftBytes = 0;
        if (len > j)
        {
            // 更新指针和长度
            ptr = (char *)(ptr + j);
            len = len - j;
        }
    }
    // 计算可整除4字节的数据块数
    count = len / 4;
    // 检查剩余字节
    i = len % 4;
    if (i > 0)
    {
        if (TotalReceived != size)
        {
            // 非最后一包，缓存剩余字节
            LeftBytes = 0;
            for (; i > 0; i--)
                LeftBytesTab[LeftBytes++] = *(char *)(ptr + len - i);
        }
        else
        {
            // 最后一包，补齐写入
            count++;
        }
    }
    // 写入整块数据
    if (count > 0)
    {
        if (Upgrade_Flash_If_Write((uint32_t *)ptr, count) == -1)
        {
            return -1;
        }
    }
    return 0;
}
