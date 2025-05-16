#ifndef _API_UPGRADE_H_
#define _API_UPGRADE_H_

#include "stdint.h"

/**
 * @brief ��ʼ�������õ�Flash�ӿ� ��������ǰ����
 * @return int  ����0�ɹ���-1ʧ��
 */
int Upgrade_Flash_If_Init(void);

/**
 * @brief ����������Flash ����ǰ�������
 * 
 */
void Upgrade_Flash_If_Erase(void);

/**
 * @brief д���ݵ�������Flash
 * 
 * @param pdata ����ָ��
 * @param len �ֽ���
 * @return int  ����0�ɹ���-1ʧ��
 */
int Upgrade_Flash_If_Write(uint32_t* pdata, int len);

/**
 * @brief ��������Flash��ȡ����
 * 
 * @param offset ƫ��
 * @param pdata ����ָ��
 * @param len �ֽ���
 * @return int  ����0�ɹ���-1ʧ��
 */
int Upgrade_Flash_If_Read(volatile unsigned int offset, unsigned char* pdata, int len);

/**
 * @brief ���ֽ�����ʽд���������ݣ�֧�ַ�4�ֽڶ���
 * 
 * @param ptr д������ָ��
 * @param len ����д�����ݳ���
 * @param TotalReceived �ۼƽ������ݳ���
 * @param size �ܴ�С
 * @return int  ����0�ɹ���-1ʧ��
 */
int Upgrade_Flash_writedata(char *ptr, uint32_t len, uint32_t TotalReceived, uint32_t size);

#endif

