#ifndef _API_UPGRADE_H_
#define _API_UPGRADE_H_

#include "stdint.h"

/**
 * @brief 初始化升级用的Flash接口 需在升级前调用
 * @return int  返回0成功，-1失败
 */
int Upgrade_Flash_If_Init(void);

/**
 * @brief 擦除升级区Flash 升级前建议调用
 * 
 */
void Upgrade_Flash_If_Erase(void);

/**
 * @brief 写数据到升级区Flash
 * 
 * @param pdata 数据指针
 * @param len 字节数
 * @return int  返回0成功，-1失败
 */
int Upgrade_Flash_If_Write(uint32_t* pdata, int len);

/**
 * @brief 从升级区Flash读取数据
 * 
 * @param offset 偏移
 * @param pdata 数据指针
 * @param len 字节数
 * @return int  返回0成功，-1失败
 */
int Upgrade_Flash_If_Read(volatile unsigned int offset, unsigned char* pdata, int len);

/**
 * @brief 按字节流方式写入升级数据，支持非4字节对齐
 * 
 * @param ptr 写入数据指针
 * @param len 本次写入数据长度
 * @param TotalReceived 累计接收数据长度
 * @param size 总大小
 * @return int  返回0成功，-1失败
 */
int Upgrade_Flash_writedata(char *ptr, uint32_t len, uint32_t TotalReceived, uint32_t size);

#endif

