#ifndef _SYS_MODULE_H_
#define _SYS_MODULE_H_

#include <stdio.h>
#include <string.h>
#include "common.h"

#define REGISTER_LEVEL_START			"0"
#define REGISTER_LEVEL_SYSTEM			"1"
#define REGISTER_LEVEL_DRIVER			"2"
#define REGISTER_LEVEL_MODULE			"3"
#define REGISTER_LEVEL_API				"4"
#define REGISTER_LEVEL_TASK				"5"
#define REGISTER_LEVEL_USER				"6"
#define REGISTER_LEVEL_END				"7"

/*匿名类型定义 -----------------------------------------------------------*/
#define  ANONY_CONN(type, var, line)  type  var##line
#define  ANONY_DEF(type,prefix,line)  ANONY_CONN(type, prefix, line)
#define  ANONY_TYPE(type,prefix)      ANONY_DEF(type, prefix, __LINE__)

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ( \
	(type *)( (char *)(ptr) - offsetof(type,member) ))


#if defined(__CC_ARM) || defined(__GNUC__) /* ARM,GCC*/
    #define SECTION(x)                  __attribute__((section(x)))
    #define UNUSED                      __attribute__((unused))
    #define USED                        __attribute__((used))
    #define ALIGN(n)                    __attribute__((aligned(n)))
    #define WEAK                        __attribute__((weak))
#elif defined (__ICCARM__)              /*IAR */
    #define SECTION(x)                  @ x
    #define UNUSED                      
    #define USED                        __root
    #define WEAK                        __weak
#else
    #error "Current tool chain haven't supported yet!"
#endif

/*系统初始化项*/
typedef struct {
		int (*init)(void);
}init_item_t;

/*模块初始化项*/
typedef struct {
    int inf_type;									
		int (*init)(void *);
}inf_item_t;

/*
 * @brief       系统初始化接口注册
 * @param[in]   ops    	- 系统操作接口指针
 * @param[in]   init	  - 系统初始化接口(void func(void){...})
 * @param[in]   level	  - 系统初始化接口排序[1-8]
 */
#define INIT_REGISTER(init,level)             \
    USED ANONY_TYPE(const init_item_t, init_item_##init)  \
    SECTION("init.item."level) =                         \
    {init}		

#define SYSTEM_INIT(init)     INIT_REGISTER(init,REGISTER_LEVEL_SYSTEM)
#define DRIVER_INIT(init)     INIT_REGISTER(init,REGISTER_LEVEL_DRIVER)
#define MODULE_INIT(init)     INIT_REGISTER(init,REGISTER_LEVEL_MODULE)
#define API_INIT(init)     		INIT_REGISTER(init,REGISTER_LEVEL_API)
#define TASK_INIT(init)     	INIT_REGISTER(init,REGISTER_LEVEL_TASK)
#define USER_INIT(init)     	INIT_REGISTER(init,REGISTER_LEVEL_USER)

/*
 * @brief       接口注册
 * @param[in]   type    - 模块类型 
 * @param[in]   init	  - 模块初始化接口(void func(void * pparams){...})
 * @param[in]   level	  - 模块初始化接口排序[1-3]
 */
#define INTERFACE_REGISTER(type,init,level)             \
    USED ANONY_TYPE(const inf_item_t, inf_item_##init)  \
    SECTION("inf.item."level) =                         \
    {type,init}
		
#define INF_REGISTER(type,init)     INTERFACE_REGISTER(type,init,REGISTER_LEVEL_DRIVER)

typedef struct {
		int (*period)(void);
}period_item_t;

/*
 * @brief       周期性接口注册
 * @param[in]   init	  - 模块初始化接口(void func(void){...})
 * @param[in]   level	  - 模块初始化接口排序[1-3]
 */
#define PERIOD_REGISTER(period,level)             \
    USED ANONY_TYPE(const period_item_t, period_item_##period)  \
    SECTION("period.item."level) =                         \
    {period}		
		
#define PER_REGISTER(period)     PERIOD_REGISTER(period,REGISTER_LEVEL_API)

typedef struct {
		void (*task)(void);
}task_item_t;

/*
 * @brief       任务创建接口注册
 * @param[in]   init	  - 模块初始化接口(void func(void){...})
 * @param[in]   level	  - 模块初始化接口排序[1-3]
 */
#define USR_TASK_REGISTER(task,level)             \
    USED ANONY_TYPE(const task_item_t, task_item_##task)  \
    SECTION("task.item."level) =                         \
    {task}	
		
#define TASK_REGISTER(task)    USR_TASK_REGISTER(task,REGISTER_LEVEL_SYSTEM)
		
void Module_Init(void);
void Task_Init(void);
void Period_Hook(void);

#endif

