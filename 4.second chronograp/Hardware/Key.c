#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//按键读取分为先读取正常值,后续进行消抖处理,最后将消抖后的结果写入Key_Num,形成Key_GetNum函数的读取结果
//后续用KeyNum读取Key_GetNum函数的结果,进行按键事件的判断,实现功能的跳转

/**
	* 函    数：按键获取键码（单次读取）
	* 参    数：无
	* 返 回 值：最近一次按键释放后产生的键码，范围：0~3；
	*           返回0表示当前没有新的按键事件
	* 说    明：读取后会清零缓存，避免同一次按键被重复读取
	*/
uint8_t Key_GetNum(void)//后续用key_num进行按键事件的判断
{
	uint8_t Temp;
	if(Key_Num)
	{
		Temp = Key_Num;//读取Key_Num,此进行过消抖操作
		Key_Num = 0;//暂存后结果清除按键事件,避免同一次按键被重复读取
		return Temp;      
	}
	return 0;
}

/**
	* 函    数：按键硬件状态读取
	* 参    数：无
	* 返 回 值：当前实时按下的键码，范围：0~3；返回0表示无按键按下
	* 说    明：该函数不做消抖和事件锁存，仅返回当前电平状态
	*/
uint8_t Key_GetState(void)//最初的案件擦欧总,后续进行消抖处理,此函数只进行按键状态的读取,不进行消抖处理,在Key_Tick函数中进行消抖处理,并将消抖后的结果写入Key_Num
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		return 1;
	}
	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 0)
	{
		return 2;
	}
	else if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 0)
	{
		return 3;
	}
	return 0;
}

/**
	* 函    数：按键扫描节拍处理
	* 参    数：无
	* 返 回 值：无
	* 说    明：周期调用（如1ms中断中调用），每20次采样一次按键状态；
	*           检测到“按下->松开”沿时，将对应键码写入Key_Num
	*/
void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrentState, PreState;
	Count++;
	if(Count >= 20)
	{
		Count = 0;
		PreState = CurrentState;
		CurrentState = Key_GetState();
		if(PreState != 0 && CurrentState == 0)
		{
			Key_Num = PreState;//将之前的状态写入Key_Num,形成Key_GetNum函数的读取结果
		}
	}
}



