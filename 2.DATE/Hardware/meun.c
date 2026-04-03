#include"stm32f10x.h"                  // Device header
#include"MyRTC.h"
#include "Key.h"
#include "LED.h"
#include "OLED.h"
#include "SetTime.h"


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
		KeyNum = Key_GetNum();//实际读取是键值,三种选项
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


/*----------------------------------设置界面-------------------------------------*/

void Show_SettingPage_UI(void){
		OLED_ShowImage(0,0,16,16,Return);
			OLED_ShowString(0,16,"日期时间设置",OLED_8X16);//查看引用
}

int setflage = 1;//设置界面标志.下面通过标志位对页面上下进行跳转
int SettingPage(void){
	while(1)
	{
		KeyNum = Key_GetNum();//实际按键值
		if(KeyNum == 1)
		{
			setflage--;
			if(setflage < 1)
			{
				setflage = 2;
			}
		}
		else if(KeyNum == 2)
		{
			setflage++;
			if(setflage > 2)
			{
				setflage = 1;
			}
		}
		else if(KeyNum == 3)
		{
			OLED_Clear();
			OLED_Update();
			return setflage;
		}//确定时返回当前标志位,以便进入对应的设置界面


 		uint8_t setflag_temp=0;
		setflag_temp = setflage;//临时变量,用于显示当前选项,用于跳转函数
		if(setflag_temp == 1){return 0;}
		else if(setflag_temp == 2){SetTime();}//跳转时间设置的界面,暂时没有设计,后续添加

		//UI界面设置,用于读取反
			switch(setflage)//UI显示区域,通过标志位进行反转显示
			{
				case 1:
				Show_SettingPage_UI();
				OLED_ReverseArea(0,0,16,16);//改动存放到缓存区,直到调用更新函数才会真正显示在屏幕上
				OLED_Update();//此为更新函数,将缓存区的改动显示在屏幕上
				break;
				
				case 2:
				Show_SettingPage_UI();
				OLED_ReverseArea(0,16,96,16);
				OLED_Update();
				break;//跳出switch,继续循环,等待按键输入
			}
			/*
			等价if语句如下（逻辑等价）：
			if (setflage == 1)
			{
				Show_SettingPage_UI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
			}
			else if (setflage == 2)
			{
				Show_SettingPage_UI();
				OLED_ReverseArea(0,16,96,16);
				OLED_Update();
			}
			*/
		
	}
}
