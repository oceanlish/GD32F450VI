#include "includes.h"

void WDT_Init(void)
{
	BSP_GPO_Init(GPO_WDT_EN);
	BSP_GPO_Init(GPO_WDT_WDI);
}

void WDT_Enable(void)
{
	BSP_GPO_Low(GPO_WDT_EN);//low Enable
}

void WDT_Disable(void)
{
	BSP_GPO_High(GPO_WDT_EN);
}


void WDT_Feed(void)
{
	BSP_GPO_Toggle(GPO_WDT_WDI);
}

