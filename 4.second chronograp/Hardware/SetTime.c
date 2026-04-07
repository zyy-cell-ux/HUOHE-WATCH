#include  "stm32f10x.h"           //包含stm32头文件
#include "MyRTC.h"
#include "Key.h"
#include "OLED.h"
#define OLED_CHARSET_UTF8//定义字符集为UTF8,如果需要使用GB2312字符集,请注释掉上面这行,并解除下面这行的注释


void Show_SetTime_FirstUI(void){
    MyRTC_ReadTime();//是否需要进行引用还是进行直接修改,何时需要进行引用
    OLED_ShowImage(0,0,16,16,Return);
    OLED_Printf(0,16,OLED_8X16,"年:%04d",MyRTC_Time[0]);
    OLED_Printf(0,32,OLED_8X16,"月:%02d",MyRTC_Time[1]);
    OLED_Printf(0,48,OLED_8X16,"日:%02d",MyRTC_Time[2]);
}


void Show_SetTime_SecondUI(void)//第一页设置与第二页设置的界面UI
{
	OLED_Printf(0,0,OLED_8X16,"时:%2d",MyRTC_Time[3]);
	OLED_Printf(0,16,OLED_8X16,"分:%2d",MyRTC_Time[4]);
	OLED_Printf(0,32,OLED_8X16,"秒:%2d",MyRTC_Time[5]);
}

void Change_RTC_Time(uint8_t i,uint8_t flag)//flag=1:加1，flag=0:减1,i作为时分秒的修改,flag作为加减的标志
{
   if (flag == 1)
   {
       MyRTC_Time[i]++;
   }
   else if (flag == 0)
   {
       MyRTC_Time[i]--;
   }
   MyRTC_SetTime();//调用RTC设置函数,将修改后的时间写入RTC
   //把你刚改过的 MyRTC_Time[i] 写回 RTC 外设寄存器。
   //让新时间真正生效并继续走时（掉电保持也依赖 RTC/BKP 机制）。
}

extern uint8_t KeyNum;//引用按键值,通过按键值进行加减操作,并调用Change_RTC_Time函数修改时间
//KeyNum = Key_GetNum();//实际按键值,在主循环中进行读取,这里不需要再次读取,直接引用即可



int SetYear(void){
while (1)//设置页持续响应按键。
{
   	KeyNum=Key_GetNum();//这里不写可以吗,前面不是已经进行定义了吗
    if (KeyNum == 1)
    {
        Change_RTC_Time(0,1);//年加1
    }
    else if (KeyNum == 2)
    {
        Change_RTC_Time(0,0);//年减1
    }
    else if (KeyNum == 3)//return0就是停止循环,返回到主界面,继续进行月的设置,否则反转的是设置年的界面
    {
        return 0;//返回主界面,到哪???年的位置反转吗
    }

    	Show_SetTime_FirstUI();
		OLED_ReverseArea(24,16,32,16);
		OLED_Update();
		//进行反转的操作
}
} 

int SetMonth(void){//开始约束范围
while (1)
{
   	KeyNum=Key_GetNum();
    if (KeyNum == 1)
    {
        Change_RTC_Time(1,1);//月加1
        if(MyRTC_Time[1]>=13){MyRTC_Time[1]=1;MyRTC_SetTime();}
    }
    else if (KeyNum == 2)
    {
        Change_RTC_Time(1,0);//月减1
        if(MyRTC_Time[1]<=0){MyRTC_Time[1]=12;MyRTC_SetTime();}//只要所有对年份的修改都通过 Change_RTC_Time 完成，就不需要再手动加 MyRTC_SetTime()。只有在你直接修改 MyRTC_Time[0]（比如跳转、溢出处理等）而没有通过 Change_RTC_Time 时，才需要手动补充 MyRTC_SetTime()。
    }
    else if (KeyNum == 3)
    {
        return 0;
    }

    	Show_SetTime_FirstUI();
        OLED_ReverseArea(24,32,32,16);
        OLED_Update();
        //进行反转的操作
}
}

int SetDay(void){
while (1)
{
   	KeyNum=Key_GetNum();
    if (KeyNum == 1)
    {
        Change_RTC_Time(2,1);//日加1
        if(MyRTC_Time[2]>=32){MyRTC_Time[2]=1;MyRTC_SetTime();}//这里没有考虑月份和闰年等因素,后续可以进行完善,是不是实际已经进行了考虑?
    }
    else if (KeyNum == 2)
    {
        Change_RTC_Time(2,0);//日减1
        if(MyRTC_Time[2]<=0){MyRTC_Time[2]=31;MyRTC_SetTime();}
    }
    else if (KeyNum == 3)
    {
        return 0;
    }

    	Show_SetTime_FirstUI();
        OLED_ReverseArea(24,48,32,16);
        OLED_Update();
        //进行反转的操作
}
}


int SetHour(void){
while (1)
{
   	KeyNum=Key_GetNum();
    if (KeyNum == 1)
    {
        Change_RTC_Time(3,1);//时加1
        if(MyRTC_Time[3]>=24){MyRTC_Time[3]=0;MyRTC_SetTime();}
    }
    else if (KeyNum == 2)
    {
        Change_RTC_Time(3,0);//时减1
        if(MyRTC_Time[3]<0){MyRTC_Time[3]=23;MyRTC_SetTime();}
    }
    else if (KeyNum == 3)
    {
        return 0;
    }

    	Show_SetTime_SecondUI();
        OLED_ReverseArea(24,0,16,16);
        OLED_Update();
        //进行反转的操作
}
}


int SetMin(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==1)//数值加1
		{
			Change_RTC_Time(4,1);
			if(MyRTC_Time[4]>=60){MyRTC_Time[4]=0;MyRTC_SetTime();}
		}
		else if(KeyNum==2)//数值减1
		{
			Change_RTC_Time(4,0);
			if(MyRTC_Time[4]<0){MyRTC_Time[4]=59;MyRTC_SetTime();}
		}
		
		else if(KeyNum==3)//确认，保存并退出
		{
			return 0;
		}
		
		Show_SetTime_SecondUI();
		OLED_ReverseArea(24,16,16,16);
		OLED_Update();
		
	}
}

int SetSec(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();//只是声明这个变量“存在”，并没有给它赋新值。KeyNum = Key_GetNum(); 才是每一轮循环去读取当前按键状态。你如果把这句去掉，KeyNum 就不会更新，if (KeyNum == 1/2/3) 这些判断也就失去意义了。
		if(KeyNum==1)//数值加1
		{
			Change_RTC_Time(5,1);
			if(MyRTC_Time[5]>=60){MyRTC_Time[5]=0;MyRTC_SetTime();}
		}
		else if(KeyNum==2)//数值减1
		{
			Change_RTC_Time(5,0);
			if(MyRTC_Time[5]<0){MyRTC_Time[5]=59;MyRTC_SetTime();}
		}
		
		else if(KeyNum==3)//确认，保存并退出
		{
			return 0;
		}
		
		Show_SetTime_SecondUI();
		OLED_ReverseArea(24,32,16,16);
		OLED_Update();
		
	}
}

int set_time_flag=1;
int SetTime(void)
{
	while(1)
	{
		KeyNum=Key_GetNum();
		uint8_t set_time_flag_temp=0;
		if(KeyNum==1)//上一项
		{
			set_time_flag--;
			if(set_time_flag<=0)set_time_flag=7;
		}
		else if(KeyNum==2)//下一项
		{
			set_time_flag++;
			if(set_time_flag>=8)set_time_flag=1;
		}
		else if(KeyNum==3)//确认
		{
			OLED_Clear();
			OLED_Update();
			set_time_flag_temp=set_time_flag;//只有确认时,才进行temp变量的赋值,以便进行函数跳转
		}
		
        //锁定按键3时候的状态进行函数跳转,执行if语句,全部结束后才进行switch语句,进行界面反转显示
		if(set_time_flag_temp==1){return 0;}//返回到上一层界面
		else if(set_time_flag_temp==2){SetYear();}//年,进入函数,此内部return0则进行返回,内部光标的判断
		else if(set_time_flag_temp==3){SetMonth();}//月
		else if(set_time_flag_temp==4){SetDay();}//日
		else if(set_time_flag_temp==5){SetHour();}//时
		else if(set_time_flag_temp==6){SetMin();}//分
		else if(set_time_flag_temp==7){SetSec();}//秒


		switch(set_time_flag)
		{
			case 1:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			
			case 2:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			
			case 3:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
			
			case 4:
				OLED_Clear();
				Show_SetTime_FirstUI();
				OLED_ReverseArea(0,48,16,16);
				OLED_Update();
				break;
			
			case 5:
				OLED_Clear();
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
				break;
			
			case 6:
				OLED_Clear();
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,16,16,16);
				OLED_Update();
				break;
			
			case 7:
				OLED_Clear();
				Show_SetTime_SecondUI();
				OLED_ReverseArea(0,32,16,16);
				OLED_Update();
				break;
		}
	}
}
