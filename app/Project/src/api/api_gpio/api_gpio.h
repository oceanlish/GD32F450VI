/**
 * @file api_gpio.h
 * @brief GPIO控制与按键管理API头文件
 * @author 
 * @date 
 * @version 1.0
 */
#ifndef _API_GPIO_H_
#define _API_GPIO_H_

#include "sys_timer.h"

#define NUM_GPIO_CTL 20           /**< GPIO控制器数量 */
#define NUM_BTN_CTL 4             /**< 按键控制器数量 */
#define GPIO_CTL_ENTRY_IDLE 0     /**< 控制表空闲 */
#define GPIO_CTL_ENTRY_USED 1     /**< 控制表已用 */

#define GPIO_CTL_IDLE 0           /**< 控制空闲状态 */
#define GPIO_CTL_ACTION 1         /**< 控制动作状态 */

#define GPIO_CTL_ACTIVE 0         /**< 激活控制 */
#define GPIO_CTL_INACTIVE 1       /**< 失活控制 */
#define GPIO_CTL_TOGGLE 2         /**< 翻转控制 */
#define GPIO_CTL_PWM 3            /**< PWM控制 */

/**
 * @brief GPIO控制初始化结构体
 */
typedef struct {
    int index;         /**< GPIO索引 */
    int init_value;    /**< 初始值 */
} ApiGpioCtlInit;

/**
 * @brief GPIO控制命令结构体
 */
typedef struct {
    int ctl_code;      /**< 执行代码 */
    int action_cnt;    /**< 执行动作次数 */
    int action_time;   /**< 动作时间 */
    int idle_time;     /**< 空闲时间 */
} ApiGpioCtl;

/**
 * @brief GPIO控制参数结构体
 */
typedef struct {
    int code;          /**< 控制代码 */
    void *ioctl;       /**< IO控制指针 */
} ApiGpioCtlParam;

/**
 * @brief GPIO控制映射表项
 */
typedef struct {
    unsigned char is_used;      /**< 是否被使用 */
    unsigned char index;        /**< GPIO索引 */
    unsigned char state;        /**< 当前状态 */
    unsigned char init_value;   /**< 默认状态 */
    int action_cnt;             /**< 剩余动作次数 */
    int action_time;            /**< 动作时间 */
    int idle_time;              /**< 空闲时间 */
    SysTimerParams last_time;   /**< 上次动作时间 */
} ApiGpioCtlMapEntry;

#define TICKS_INTERVAL 5         /**< 定时器间隔(ms) */
#define DEBOUNCE_TICKS 3         /**< 消抖周期，最大8 */
#define SHORT_TICKS (300 / TICKS_INTERVAL)
#define LONG_TICKS (3000 / TICKS_INTERVAL)
#define EVENT_CB(ev)    \
    if (pEntry->cb[ev]) \
    pEntry->cb[ev](pEntry)

/**
 * @brief 按键回调函数类型
 */
typedef void (*BtnCallback)(void *);

/**
 * @brief 按键事件枚举
 */
typedef enum {
    PRESS_DOWN = 0,         /**< 按下 */
    PRESS_UP,               /**< 松开 */
    PRESS_REPEAT,           /**< 连续按下 */
    SINGLE_CLICK,           /**< 单击 */
    DOUBLE_CLICK,           /**< 双击 */
    LONG_PRESS_START,       /**< 长按开始 */
    LONG_PRESS_HOLD,        /**< 长按保持 */
    NUM_OF_EVENTS,          /**< 事件数量 */
    NONE_PRESS              /**< 无事件 */
} PressEvent;

/**
 * @brief 按键映射表项
 */
typedef struct {
    unsigned char is_used;          /**< 是否被使用 */
    unsigned char index;            /**< 按键索引 */
    unsigned char repeat : 4;       /**< 连续次数 */
    unsigned char event : 4;        /**< 当前事件 */
    unsigned char state : 3;        /**< 状态 */
    unsigned char debounce_cnt : 3; /**< 消抖计数 */
    unsigned char active_level : 1; /**< 有效电平 */
    unsigned char button_level : 1; /**< 当前电平 */
    unsigned int ticks;             /**< 计时 */
    BtnCallback cb[NUM_OF_EVENTS];  /**< 事件回调 */
} ApiButtonMapEntry;

/**
 * @brief GPIO控制映射表
 */
typedef struct {
    int s_iLock;                        /**< 互斥锁 */
    int iClientNum;                     /**< 客户端数量 */
    SysTimerParams timer;               /**< 定时器参数 */
    ApiGpioCtlMapEntry Entry[NUM_GPIO_CTL];    /**< GPIO控制表 */
    ApiButtonMapEntry BtnEntry[NUM_BTN_CTL];   /**< 按键表 */
} ApiGpioCtlMap;

/**
 * @brief GPIO主结构体
 */
typedef struct {
    void *gpio_fd;              /**< 设备句柄 */
    ApiGpioCtlMap map;          /**< 控制映射表 */
} ApiGpio;

/**
 * @brief GPIO操作函数表
 */
typedef struct {
    int (*gpio_read)(int);
    int (*gpio_write)(int, int);
    int (*gpio_toggle)(int);
    int (*gpo_ioctl_init)(void **, ApiGpioCtlInit *);
    int (*gpo_ioctl)(void *, ApiGpioCtl *);
    int (*gpo_ioctl_deinit)(void *);
    int (*gpi_btn_init)(void **, ApiGpioCtlInit *);
    int (*gpi_btn_deinit)(void *);
    int (*gpi_btn_attach)(void *, PressEvent, BtnCallback);
    int (*gpi_btn_event)(void *);
    int (*gpio_ioctl_period)(void);
} ApiGpioOps;

#endif
