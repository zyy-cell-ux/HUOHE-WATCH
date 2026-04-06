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
int SettingPage(void){//内部也用while1进行循环,定义SettingPage函数
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
		if(setflag_temp == 1){return 0;}//返回到主界面
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

//滑动菜单函数
//图标移动,上次选择的选项变为这次选择的选项,框不发生变化
uint8_t pre_selection;//上次选择选项
uint8_t target_selection;//目标选项,以索引值进行变化,Menu_Graph[i][128];
uint8_t x_pre=48;//上次选项的x坐标,框的左上角x的值
uint8_t Speed=4;//每次移动的像素数,越大越快,但过大会导致动画不流畅
uint8_t move_flag;//是否开始移动的标志位,0为不移动,1为移动

//外部循环不断调用,刷新,造成动画效果
void Menu_Animation(void)
{
	OLED_Clear();
	OLED_ShowImage(42,10,44,44,Frame);
	//实际的逻辑函数如下：
	if(pre_selection<target_selection)//目标选项在右边,所有的要向左移动
	{
		x_pre-=Speed;//上次坐标开始左移,一直减到0
		if(x_pre==0)//原本中间位置的值移动到最左边,目标选项已经移动到中间位置,动画结束
		{
			pre_selection++;//改变中间即上次选项的值/变为target_selection,索引加1
			move_flag=0;//动画结束,停止移动
			x_pre=48;//坐标重置,准备下一次动画
		}
	}
	
	if(pre_selection>target_selection)
	{
		x_pre+=Speed;//上次坐标开始右移,一直加到96
		if(x_pre==96)
		{
			pre_selection--;
			move_flag=0;
			x_pre=48;
		}
	}

	//上面移动判断结束,开始为UI界面显示图标
	//整个界面以pre_selection为基准进行显示,pre_selection的左右两边分别是pre_selection-1和pre_selection+1,以此类推
	if(pre_selection>=1)
	{
		OLED_ShowImage(x_pre-48,16,32,32,Menu_Graph[pre_selection-1]);
	}
	
	if(pre_selection>=2)
	{
		OLED_ShowImage(x_pre-96,16,32,32,Menu_Graph[pre_selection-2]);
	}
	
	OLED_ShowImage(x_pre,16,32,32,Menu_Graph[pre_selection]);
	
	if(pre_selection<=5)
	{
		OLED_ShowImage(x_pre+48,16,32,32,Menu_Graph[pre_selection+1]);
	}
	
	if(pre_selection<=4)
	{
		OLED_ShowImage(x_pre+96,16,32,32,Menu_Graph[pre_selection+2]);
	}
	
	OLED_ShowImage(x_pre,16,32,32,Menu_Graph[pre_selection]);//显示的图标始终是pre_selection,以pre_selection为基准进行显示,左右两边分别是pre_selection-1和pre_selection+1,以此类推
	OLED_ShowImage(x_pre+48,16,32,32,Menu_Graph[pre_selection+1]);
	OLED_ShowImage(x_pre+96,16,32,32,Menu_Graph[pre_selection+2]);
	
	OLED_Update();
}

void Set_Selection(uint8_t move_flag,uint8_t Pre_Selection,uint8_t Target_Selection)//按键设定
{
	if(move_flag==1)
	{
		pre_selection=Pre_Selection;//全局变量等于传进来的
		target_selection=Target_Selection;//同理
		//调用动画函数,在外部循环不断调用,刷新,造成动画效果,参数已经被定义
		Menu_Animation();
	}
}

uint8_t menu_flag=1;
int Menu(void)
{
	move_flag=1;//是否在移动
	uint8_t DirectFlag=2;//1为上一项,2为下一项
	while(1)
	{
		//菜单的逻辑判断函数
		KeyNum=Key_GetNum();//读取按键值
		uint8_t menu_flag_temp=0;
		if(KeyNum==1)
		{
			DirectFlag=1;//按键1为上一项,按键2为下一项
			move_flag=1;//确定移动
			menu_flag--;//菜单的标志位,一共7个菜单选项,表示第几个图标
			if(menu_flag<=0)menu_flag=7;
		}
		else if(KeyNum==2)
		{
			DirectFlag=2;//按键1为上一项,按键2为下一项
			move_flag=1;//确定移动
			menu_flag++;
			if(menu_flag>=8)menu_flag=1;
		}
		else if(KeyNum==3)
		{
			OLED_Clear();
			OLED_Update();
			menu_flag_temp=menu_flag;//确定时,保留temp进入下一层函数判断
		}
		
		if(menu_flag_temp==1){return 0;}
		else if(menu_flag_temp==2){}
		else if(menu_flag_temp==3){}
	  else if(menu_flag_temp==4){}
		else if(menu_flag_temp==5){}
		else if(menu_flag_temp==6){}
		else if(menu_flag_temp==7){}
			

			if(menu_flag==1)//此为页面跳转函数
			{
				if(DirectFlag==1)Set_Selection(move_flag,1,0);//while内部,对Menu_Animation的参数进行设定,以实现动画效果,参数已经被定义
				//Set_Selection函数的参数分别为move_flag,Pre_Selection,Target_Selection
				//定义KeyNum==1,DirectFlag==1,move_flag==1时进行绑定菜园子单选项的跳转,Pre_Selection和Target_Selection分别为当前选项和目标选项的索引值,以此类推
				else if(DirectFlag==2)Set_Selection(move_flag,0,0);//此时Pre_Selection和Target_Selection相同,因为没有上一项了,所以保持不变
			}
			
			else//考虑左移还是右移的情况
			{
				if(DirectFlag==1)Set_Selection(move_flag,menu_flag,menu_flag-1);//整体右移
				//此时Pre_Selection为当前选项的索引值,Target_Selection为当前选项的索引值减1,以此类推
				else if(DirectFlag==2)Set_Selection(move_flag,menu_flag-2,menu_flag-1);
			}

			//等价于switch语句如下（逻辑等价）：
			/*switch(menu_flag){
			
			case 1:
				if(DirectFlag==1)Set_Selection(move_flag,1,0);
				else if(DirectFlag==2)Set_Selection(move_flag,0,0);
				break;
				case 2:
				if(DirectFlag==1)Set_Selection(move_flag,2,1);
				else if(DirectFlag==2)Set_Selection(move_flag,1,0);
				break;
				case 3:
				if(DirectFlag==1)Set_Selection(move_flag,3,2);
				else if(DirectFlag==2)Set_Selection(move_flag,2,1);
				break;
				case 4:
					if(DirectFlag==1)Set_Selection(move_flag,4,3);
				else if(DirectFlag==2)Set_Selection(move_flag,3,2);
				break;
				case 5:
					if(DirectFlag==1)Set_Selection(move_flag,5,4);
				else if(DirectFlag==2)Set_Selection(move_flag,4,3);
				break;
				case 6:
					if(DirectFlag==1)Set_Selection(move_flag,6,5);
				else if(DirectFlag==2)Set_Selection(move_flag,5,4);
				break;
				case 7:
					if(DirectFlag==1)Set_Selection(move_flag,7,6);
				else if(DirectFlag==2)Set_Selection(move_flag,6,5);
				break;
				}
				*/
	}
}