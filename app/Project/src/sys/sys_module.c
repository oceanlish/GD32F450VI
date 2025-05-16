#include "dbg_log.h"
#include "sys_module.h"

const init_item_t init_tbl_start	SECTION("init.item."REGISTER_LEVEL_START) = { NULL };
const init_item_t init_tbl_end		SECTION("init.item."REGISTER_LEVEL_END)		= { NULL };
const period_item_t period_tbl_start	SECTION("period.item."REGISTER_LEVEL_START) = { NULL };
const period_item_t period_tbl_end		SECTION("period.item."REGISTER_LEVEL_END)		= { NULL };
const task_item_t task_tbl_start	SECTION("task.item."REGISTER_LEVEL_START) = { NULL };
const task_item_t task_tbl_end		SECTION("task.item."REGISTER_LEVEL_END)		= { NULL };
/*
 * @brief       模块初始处理
 *              初始化模块优化级 system_init > driver_init > module_init
 * @param[in]   none
 * @return      none
 */
void Module_Init(void)
{
		const init_item_t *it = &init_tbl_start;
		while (it < &init_tbl_end) 
		{
			if(it->init != NULL)
			{
				it->init();
			}
			it++;
		}
}

/*
 * @brief       周期性任务处理
 * @param[in]   none
 * @return      none
 */
void Period_Hook(void)
{
		const period_item_t *it = &period_tbl_start;
		while (it < &period_tbl_end) 
		{
			if(it->period!= NULL)
			{
				it->period();
			}
			it++;
		}
}

void Task_Init(void)
{
		const task_item_t *it = &task_tbl_start;
		while (it < &task_tbl_end) 
		{
			if(it->task!= NULL)
			{
				it->task();
			}
			it++;
		}
}

