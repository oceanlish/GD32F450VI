/**
 * @file api_gpio.c
 * @brief GPIO�����밴������APIʵ���ļ�
 * @author 
 * @date 
 * @version 1.0
 */
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "common.h"
#include "interface.h"
#include "api_gpio.h"

/**
 * @brief GPIO���ṹ�徲̬ʵ��
 */
static ApiGpio s_ApiGpio;

/**
 * @brief ��ȡָ��GPIO�ĵ�ƽֵ
 * @param index GPIO����
 * @return ��ȡ���ĵ�ƽֵ��-1��ʾʧ��
 */
int Api_Gpio_Read(int index)
{
    GpioParam stGpioParam;
    stGpioParam.pllif_fd = s_ApiGpio.gpio_fd;
    stGpioParam.index = index;
    if (-1 == g_stGlobeOps.if_ops.read(&stGpioParam)) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpio Open error!\n");
        return -1;
    }
    return stGpioParam.value;
}

/**
 * @brief ����ָ��GPIO�ĵ�ƽֵ
 * @param index GPIO����
 * @param value ���õĵ�ƽֵ
 * @return 0�ɹ���-1ʧ��
 */
int Api_Gpio_Write(int index, int value)
{
    GpioParam stGpioParam;
    stGpioParam.pllif_fd = s_ApiGpio.gpio_fd;
    stGpioParam.index = index;
    stGpioParam.value = value;
    if (-1 == g_stGlobeOps.if_ops.write(&stGpioParam)) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpio Open error!\n");
        return -1;
    }
    return 0;
}

/**
 * @brief ��תָ��GPIO�ĵ�ƽֵ
 * @param index GPIO����
 * @return 0�ɹ���-1ʧ��
 */
int Api_Gpio_Toggle(int index)
{
    GpioParam stGpioParam;
    stGpioParam.pllif_fd = s_ApiGpio.gpio_fd;
    stGpioParam.index = index;
    stGpioParam.value = GPIO_TOGGLE;
    if (-1 == g_stGlobeOps.if_ops.write(&stGpioParam)) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpio Open error!\n");
        return -1;
    }
    return 0;
}

/**
 * @brief ��ʼ��GPIO������
 * @param[out] ioctl_fd IO���ƾ��
 * @param[in] init ��ʼ������
 * @return 0�ɹ���-1ʧ��
 */
int Api_Gpo_Ctl_Init(void **ioctl_fd, ApiGpioCtlInit *init)
{
    int index = 0;
    g_stGlobeOps.mutex_ops.lock(s_ApiGpio.map.s_iLock, 0);
    for (index = 0; index < NUM_GPIO_CTL; index++) {
        if (s_ApiGpio.map.Entry[index].is_used == GPIO_CTL_ENTRY_IDLE) {
            s_ApiGpio.map.Entry[index].is_used = GPIO_CTL_ENTRY_USED;
            break;
        }
    }
    if (index == NUM_GPIO_CTL) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpo Ctl Init Error, No Space!\n");
        g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
        return -1;
    }
    if ((init->init_value != GPIO_HIGH) && (init->init_value != GPIO_LOW)) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpo Ctl Init Value Error!\n");
        s_ApiGpio.map.Entry[index].is_used = GPIO_CTL_ENTRY_IDLE;
        g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
        return -1;
    }
    s_ApiGpio.map.Entry[index].index = init->index;
    s_ApiGpio.map.Entry[index].init_value = init->init_value;
    *ioctl_fd = &s_ApiGpio.map.Entry[index];
    s_ApiGpio.map.iClientNum++;
    Api_Gpio_Write(init->index, init->init_value);
    g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
    return 0;
}

/**
 * @brief �ͷ�GPIO������
 * @param ioctl_fd IO���ƾ��
 * @return 0
 */
int Api_Gpo_Ctl_DeInit(void *ioctl_fd)
{
    ApiGpioCtlMapEntry *pEntry = (ApiGpioCtlMapEntry *)ioctl_fd;
    g_stGlobeOps.mutex_ops.lock(s_ApiGpio.map.s_iLock, 0);
    memset(pEntry, 0, sizeof(ApiGpioCtlMapEntry));
    s_ApiGpio.map.iClientNum--;
    g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
    return 0;
}

/**
 * @brief ִ��GPIO��������
 * @param ioctl_fd IO���ƾ��
 * @param ctl ��������
 * @return 0�ɹ���-1ʧ��
 */
int Api_Gpo_Ctl(void *ioctl_fd, ApiGpioCtl *ctl)
{
    ApiGpioCtlMapEntry *pEntry = (ApiGpioCtlMapEntry *)ioctl_fd;
    if ((ctl->ctl_code == GPIO_CTL_PWM) && (ctl->action_cnt == 0)) {
        return -1;
    }
    switch (ctl->ctl_code) {
    case GPIO_CTL_ACTIVE:
        pEntry->state = GPIO_CTL_IDLE;
        pEntry->action_cnt = 0;
        if (pEntry->init_value == GPIO_HIGH) {
            Api_Gpio_Write(pEntry->index, GPIO_LOW);
        } else if (pEntry->init_value == GPIO_LOW) {
            Api_Gpio_Write(pEntry->index, GPIO_HIGH);
        } else {
            logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpo Ctl Value Error!\n");
            return -1;
        }
        break;
    case GPIO_CTL_INACTIVE:
        pEntry->state = GPIO_CTL_IDLE;
        pEntry->action_cnt = 0;
        if (pEntry->init_value == GPIO_HIGH) {
            Api_Gpio_Write(pEntry->index, GPIO_HIGH);
        } else if (pEntry->init_value == GPIO_LOW) {
            Api_Gpio_Write(pEntry->index, GPIO_LOW);
        } else {
            logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpo Ctl Value Error!\n");
            return -1;
        }
        break;
    case GPIO_CTL_TOGGLE:
        pEntry->state = GPIO_CTL_IDLE;
        pEntry->action_cnt = 0;
        Api_Gpio_Toggle(pEntry->index);
        break;
    case GPIO_CTL_PWM:
        pEntry->state = GPIO_CTL_ACTION;
        pEntry->action_cnt = ctl->action_cnt;
        pEntry->action_time = ctl->action_time;
        pEntry->idle_time = ctl->idle_time;
        g_stGlobeOps.systimer_ops.get_runtime(&pEntry->last_time);
        if (pEntry->action_cnt > 0) {
            pEntry->action_cnt--;
        }
        if (pEntry->init_value == GPIO_HIGH) {
            Api_Gpio_Write(pEntry->index, GPIO_LOW);
        } else if (pEntry->init_value == GPIO_LOW) {
            Api_Gpio_Write(pEntry->index, GPIO_HIGH);
        } else {
            logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpo Ctl Value Error!\n");
            return -1;
        }
        break;
    default:
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Unknown Ctrl Code!\n");
        break;
    }
    return 0;
}

/**
 * @brief GPIO�������ڴ���
 * @param pEntry ���Ʊ���ָ��
 */
void Api_Gpo_Period(ApiGpioCtlMapEntry *pEntry)
{
    switch (pEntry->state) {
    case GPIO_CTL_ACTION:
        if (g_stGlobeOps.systimer_ops.diff_runtime(&pEntry->last_time) < pEntry->action_time) {
            return;
        }
        pEntry->state = GPIO_CTL_IDLE;
        if (pEntry->idle_time == 0) {
            return;
        }
        Api_Gpio_Write(pEntry->index, pEntry->init_value);
        g_stGlobeOps.systimer_ops.get_runtime(&pEntry->last_time);
        break;
    case GPIO_CTL_IDLE:
        if (pEntry->action_cnt == 0) {
            return;
        }
        if (g_stGlobeOps.systimer_ops.diff_runtime(&pEntry->last_time) < pEntry->idle_time) {
            return;
        }
        pEntry->state = GPIO_CTL_ACTION;
        if (pEntry->action_cnt > 0) {
            pEntry->action_cnt--;
        }
        if (pEntry->init_value == GPIO_HIGH) {
            Api_Gpio_Write(pEntry->index, GPIO_LOW);
        } else if (pEntry->init_value == GPIO_LOW) {
            Api_Gpio_Write(pEntry->index, GPIO_HIGH);
        }
        g_stGlobeOps.systimer_ops.get_runtime(&pEntry->last_time);
        break;
    default:
        break;
    }
}

/**
 * @brief ��ʼ������������
 * @param[out] ioctl_fd IO���ƾ��
 * @param[in] init ��ʼ������
 * @return 0�ɹ���-1ʧ��
 */
int Api_Gpi_Btn_Init(void **ioctl_fd, ApiGpioCtlInit *init)
{
    int index = 0;
    g_stGlobeOps.mutex_ops.lock(s_ApiGpio.map.s_iLock, 0);
    for (index = 0; index < NUM_BTN_CTL; index++) {
        if (s_ApiGpio.map.BtnEntry[index].is_used == GPIO_CTL_ENTRY_IDLE) {
            memset(&s_ApiGpio.map.BtnEntry[index], 0, sizeof(s_ApiGpio.map.BtnEntry[index]));
            s_ApiGpio.map.BtnEntry[index].is_used = GPIO_CTL_ENTRY_USED;
            break;
        }
    }
    if (index == NUM_BTN_CTL) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpi Ctl Init Error, No Space!\n");
        g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
        return -1;
    }
    if ((init->init_value != GPIO_HIGH) && (init->init_value != GPIO_LOW)) {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpi Ctl Init Value Error!\n");
        s_ApiGpio.map.BtnEntry[index].is_used = GPIO_CTL_ENTRY_IDLE;
        g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
        return -1;
    }
    s_ApiGpio.map.BtnEntry[index].event = (unsigned char)NONE_PRESS;
    s_ApiGpio.map.BtnEntry[index].index = init->index;
    s_ApiGpio.map.BtnEntry[index].button_level = Api_Gpio_Read(init->index);
    s_ApiGpio.map.BtnEntry[index].active_level = init->init_value;
    *ioctl_fd = &s_ApiGpio.map.BtnEntry[index];
    s_ApiGpio.map.iClientNum++;
    g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
    return 0;
}

/**
 * @brief �ͷŰ���������
 * @param ioctl_fd IO���ƾ��
 * @return 0
 */
int Api_Gpi_Btn_DeInit(void *ioctl_fd)
{
    ApiButtonMapEntry *pEntry = (ApiButtonMapEntry *)ioctl_fd;
    g_stGlobeOps.mutex_ops.lock(s_ApiGpio.map.s_iLock, 0);
    memset(pEntry, 0, sizeof(ApiButtonMapEntry));
    s_ApiGpio.map.iClientNum--;
    g_stGlobeOps.mutex_ops.unlock(s_ApiGpio.map.s_iLock);
    return 0;
}

/**
 * @brief �����¼��ص���
 * @param ioctl_fd IO���ƾ��
 * @param event �¼�����
 * @param cb �ص�����
 * @return 0
 */
int Api_Gpi_Btn_Attach(void *ioctl_fd, PressEvent event, BtnCallback cb)
{
    ApiButtonMapEntry *pEntry = (ApiButtonMapEntry *)ioctl_fd;
    pEntry->cb[event] = cb;
    return 0;
}

/**
 * @brief ��ȡ������ǰ�¼�
 * @param ioctl_fd IO���ƾ��
 * @return ��ǰ�¼�
 */
int Api_Gpi_Btn_Event(void *ioctl_fd)
{
    ApiButtonMapEntry *pEntry = (ApiButtonMapEntry *)ioctl_fd;
    return pEntry->event;
}

/**
 * @brief �������ڴ���
 * @param pEntry ��������ָ��
 */
void Api_Gpi_Btn_Period(ApiButtonMapEntry *pEntry)
{
    unsigned char read_gpio_level = Api_Gpio_Read(pEntry->index);
    if ((pEntry->state) > 0)
        pEntry->ticks++;
    if (read_gpio_level != pEntry->button_level) {
        if (++(pEntry->debounce_cnt) >= DEBOUNCE_TICKS) {
            pEntry->button_level = read_gpio_level;
            pEntry->debounce_cnt = 0;
        }
    } else {
        pEntry->debounce_cnt = 0;
    }
    switch (pEntry->state) {
    case 0:
        if (pEntry->button_level == pEntry->active_level) {
            logs(DBG_GPIO_SW, DBG_INFO, DBG_COLOR_YELLOW, DBG_TS_EN, "[Api Gpio] Gpi Btn Event [PRESS_DOWN]!\n");
            // �������չ�¼�����
        }
        break;
    default:
        break;
    }
}

/**
 * @brief GPIO�������ڴ�����
 * @return 0�ɹ�
 */
int Api_Gpio_Ctl_Period(void)
{
    int index = 0;

    // ����Ƿ񵽴����ڴ���ʱ����
    if (g_stGlobeOps.systimer_ops.diff_runtime(&s_ApiGpio.map.timer) >= TICKS_INTERVAL)
    {
        // ���¶�ʱ��ʱ���
        g_stGlobeOps.systimer_ops.get_runtime(&s_ApiGpio.map.timer);

        // �������а���������,�������¼�
        for (index = 0; index < NUM_BTN_CTL; index++)
        {
            // ��鰴���������Ƿ���ʹ����
            if (s_ApiGpio.map.BtnEntry[index].is_used == GPIO_CTL_ENTRY_USED)
            {
                // ִ�а������ڴ���
                Api_Gpi_Btn_Period(&s_ApiGpio.map.BtnEntry[index]);
            }
        }

        // ��������GPIO������,����GPIO�¼�
        for (index = 0; index < NUM_GPIO_CTL; index++)
        {
            // ���GPIO�������Ƿ���ʹ����
            if (s_ApiGpio.map.Entry[index].is_used == GPIO_CTL_ENTRY_USED)
            {
                // ִ��GPIO���ڴ���
                Api_Gpo_Period(&s_ApiGpio.map.Entry[index]);
            }
        }
    }

    return 0;
}

int Api_Gpio_Init(void)
{
    llifParams params;

    // ��ӡ���뺯������־
    logs(DBG_GPIO_SW, DBG_DETAIL, DBG_COLOR_NULL, DBG_TS_EN, "[Api Gpio] Enter %s\n", __FUNCTION__);
    
    // ��ʼ��GPIO���ṹ��
    memset(&s_ApiGpio, 0, sizeof(s_ApiGpio));
    
    // ��ȡϵͳʱ��
    g_stGlobeOps.systimer_ops.get_runtime(&s_ApiGpio.map.timer);
    
    // ����GPIO�ӿ�����
    params.inf_type = INF_GPIO;

    // ��GPIO�豸
    if (-1 == g_stGlobeOps.if_ops.open(&params, &s_ApiGpio.gpio_fd))
    {
        logs(DBG_GPIO_SW, DBG_ERR, DBG_COLOR_RED, DBG_TS_EN, "[Api Gpio] Gpio Open error!\n");
        return -1;
    }

    // ��ʼ��������
    g_stGlobeOps.mutex_ops.init("api_lock", &s_ApiGpio.map.s_iLock);

    // ע��GPIO��������
    g_stGlobeOps.gpio_ops.gpio_read = Api_Gpio_Read;          // GPIO��ȡ
    g_stGlobeOps.gpio_ops.gpio_write = Api_Gpio_Write;        // GPIOд��
    g_stGlobeOps.gpio_ops.gpio_toggle = Api_Gpio_Toggle;      // GPIO��ת
    g_stGlobeOps.gpio_ops.gpo_ioctl_init = Api_Gpo_Ctl_Init;    // GPIO������Ƴ�ʼ��
    g_stGlobeOps.gpio_ops.gpo_ioctl_deinit = Api_Gpo_Ctl_DeInit; // GPIO������Ʒ���ʼ��
    g_stGlobeOps.gpio_ops.gpo_ioctl = Api_Gpo_Ctl;           // GPIO�������
    g_stGlobeOps.gpio_ops.gpi_btn_init = Api_Gpi_Btn_Init;     // ������ʼ��
    g_stGlobeOps.gpio_ops.gpi_btn_deinit = Api_Gpi_Btn_DeInit;  // ��������ʼ��
    g_stGlobeOps.gpio_ops.gpi_btn_attach = Api_Gpi_Btn_Attach;  // �����¼���
    g_stGlobeOps.gpio_ops.gpi_btn_event = Api_Gpi_Btn_Event;    // �����¼���ȡ

    // ע��GPIO���ڴ�����
    g_stGlobeOps.gpio_ops.gpio_ioctl_period = Api_Gpio_Ctl_Period;

    return 0;
}
API_INIT(Api_Gpio_Init);
