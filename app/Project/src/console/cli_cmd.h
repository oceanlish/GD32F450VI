#ifndef _CLI_CMD_H_
#define _CLI_CMD_H_

#include <stdio.h>
#include <string.h>
#include "common.h"

#define CONFIG_LINKER_LIST_ALIGN 4
	


#define DEBUG_CMD_START			"0"
#define DEBUG_CMD_INIT			"1"
#define DEBUG_CMD_END			"2"

enum command_ret_t {
	CMD_RET_SUCCESS,	/* 0 = Success */
	CMD_RET_FAILURE,	/* 1 = Failure */
	CMD_RET_USAGE = -1,	/* Failure, please report 'usage' error */
};

struct cmd_tbl {
	char        *name;        /* Command Name            */
	int          maxargs;    /* maximum number of arguments    */
	int          repeatable;    /* autorepeat allowed?        */
	/* Implementation function    */
	int        (*cmd)(struct cmd_tbl *, int, int, char * const []);
	char        *usage;        /* Usage message    (short)    */

	char        *help;        /* Help  message    (long)    */

#ifdef CONFIG_AUTO_COMPLETE
	/* do auto completion on the arguments */
	int        (*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]);
#endif
};
typedef struct cmd_tbl    cmd_tbl_s;



struct cmd_tbl_t
{
	struct cmd_tbl cmd;
};

typedef struct {
		struct cmd_tbl *p_cmd_tbl;
}debug_cmd;


//#define  ANONY_CONN(type, var, line)  type  var##line
//#define  ANONY_DEF(type,prefix,line)  ANONY_CONN(type, prefix, line)
//#define  ANONY_TYPE(type,prefix)      ANONY_DEF(type, prefix, __LINE__)

//#if defined(__CC_ARM) || defined(__GNUC__) /* ARM,GCC*/
//    #define SECTION(x)                  __attribute__((section(x)))
//    #define UNUSED                      __attribute__((unused))
//    #define USED                        __attribute__((used))
//    #define ALIGN(n)                    __attribute__((aligned(n)))
//    #define WEAK                        __attribute__((weak))
//#elif defined (__ICCARM__)              /*IAR */
//    #define SECTION(x)                  @ x
//    #define UNUSED                      
//    #define USED                        __root
//    #define WEAK                        __weak
//#else
//    #error "Current tool chain haven't supported yet!"
//#endif
/*
 * @brief       周期性接口注册
 * @param[in]   init	  - 模块初始化接口(void func(void){...})
 * @param[in]   level	  - 模块初始化接口排序[1-3]
 */
//#define DEBUG_CMD_REGISTER(cmd,level)             \
//    USED ANONY_TYPE(const struct cmd_tbl_t, debug_cmd_##cmd)  \
//    SECTION("debug_cmd.item."level) =                         \
//    {cmd}		
//		
//#define CMD_REGISTER(cmd)     DEBUG_CMD_REGISTER(cmd,DEBUG_CMD_INIT)

#define Struct_Section  __attribute__ ((used,section ("debug_cmd.item.1")))	
	
	
	
#define U_BOOT_CMD(name,maxargs,rep,cmd,usage,help) \
	cmd_tbl_s __u_boot_cmd_##name Struct_Section = {#name, maxargs, rep, cmd, usage, help}

#define MAX_ARGC_COUNT	10
typedef struct{
	int argc;
	char *argv[MAX_ARGC_COUNT];
}arg_info;	

#define isblank(c)	(c == ' ' || c == '\t')

struct cmd_tbl *find_cmd(const char *cmd,arg_info *info,int flag);
int cmd_parse_info(char *line,arg_info *info);


extern cmd_tbl_s cmd_tbl_end;
extern cmd_tbl_s cmd_tbl_start;
#endif

