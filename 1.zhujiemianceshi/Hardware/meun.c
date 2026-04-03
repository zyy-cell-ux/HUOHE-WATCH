#include"stm32f10x.h"                  // Device header
#include"MyRTC.h"
#include "Key.h"
#include "LED.h"
#include "OLED.h"

uint8_t KeyNum = 0;

void Peripheral_Init(void)
{
	MyRTC_Init();
	Key_Init();
	LED_Init();
}

void Show_Clock_UI(void)
{
	MyRTC_ReadTime();
	OLED_Printf(0, 0, OLED_6X8, "%04d/%02d/%02d", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]);
	OLED_Printf(16, 16, OLED_12X24, "%02d:%02d:%02d", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);
	OLED_ShowString(0, 48, "菜单", OLED_8X16);
	OLED_ShowString(64, 48, "设置", OLED_8X16);
}

int clflage = 1;

int Fist_Page_Clock(void)
{
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			clflage--;
			if(clflage < 1)
			{
				clflage = 2;
			}
		}
		else if(KeyNum == 2)
		{
			clflage++;
			if(clflage > 2)
			{
				clflage = 1;
			}
		}
		else if(KeyNum == 3)
		{
			OLED_Clear();
			OLED_Update();
			return clflage;
		}
	}
}
