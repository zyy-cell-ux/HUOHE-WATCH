#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "OLED.h"
#include "Timer.h"
#include "meun.h"

/**
  * 坐标轴定义：
  * 左上角为(0, 0)点
  * 横向向右为X轴，取值范围：0~127
  * 纵向向下为Y轴，取值范围：0~63
  * 
  *       0             X轴           127 
  *      .------------------------------->
  *    0 |
  *      |
  *      |
  *      |
  *  Y轴 |
  *      |
  *      |
  *      |
  *   63 |
  *      v
  * 
  */

//删除oled测试程序
int clkflag1;//用于保存First_Page_Clock()的返回值,根据返回值进行页面跳转

int main(void)
{
	/*OLED初始化*/
	OLED_Init();
	OLED_Clear();
	Peripheral_Init();
	
	
	Timer_Init();
	while (1)
	{
		clkflag1=Fist_Page_Clock();//clkflag1保存First_Page_Clock()的返回值,根据返回值进行页面跳转
		if(clkflag1==1){}//菜单,后续进行跳转的函数
		else if(clkflag1==2){}//设置
	}
}


void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Key_Tick();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}//每20ms进入一次TIM2中断,在中断函数里调用Key_Tick函数,进行按键状态的更新,从而实现按键的扫描和消抖功能
