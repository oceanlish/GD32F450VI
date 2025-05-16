#ifndef _WDT_H
#define _WDT_H
#include "includes.h"


extern void WDT_Init(void);
extern void WDT_Enable(void);
extern void WDT_Disable(void);
extern void WDT_Feed(void);

#endif

