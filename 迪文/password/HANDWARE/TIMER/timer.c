/******************************************************************************

                  版权所有 (C), 2019, 北京迪文科技有限公司

 ******************************************************************************
  文 件 名   : timer.c
  版 本 号   : V1.0
  作    者   : chenmeishu
  生成日期   : 2019.9.2
  功能描述   : 实现了定时器    硬件定时器0    实现了8个软定时器
  修改历史   :
  1.日    期   : 
    作    者   : 
    修改内容   : 
******************************************************************************/
#include "sys.h"
#include "timer.h"





//注意：禁止私自访问这些变量，只能用接口函数访问
//u8  EnableTimer;   //8个软定时器是否使能
//u8  OutTimeFlag;   //8个软定时器是否超时
//u32 TimerTime[8];  //8个软定时器定时时间







u8  EnablePWM;
u16 PWMTotal[8];
u16 PWMLow[8];
u16 PWMTicks[8];
u8  PWMPort[8];
u8  PWMPin[8];


//注意：禁止私自访问这些变量，只能用接口函数访问
u8  EnableTimer;   //8个软定时器是否使能
u8  OutTimeFlag;   //8个软定时器是否超时
u16 TimerTime[8];  //8个软定时器定时时间

u32 secondcnt;    //开机以来的秒数
u32 usecondcnt;   //开机以来的微秒

u32 msecondcnt;










//获得开机以来的秒数和微妙数
/*

void GetSystemTime(u32 *psecond,u32 *pusecond)
{
	  EA=0;
	  *psecond=secondcnt;	
	  *pusecond=usecondcnt;
	  EA=1;
	
}
*/
void GetSystemms(u32 *pmsecond)
{
		EA=0;	  
	  *pmsecond=msecondcnt;
	  EA=1;
}

/*

void T2_Init(void)
{

	TH2 = 0xff;
	TL2 = 0xff;
	TRL2H = T1US>>8;
	TRL2L = T1US;		
	T2CON = 0x71;		
	ET2 = 1;		
  secondcnt=0;	
	usecondcnt=0;
}

void timer2_Isr() interrupt 5
{
	EA = 0;
	TF2 = 0;
  usecondcnt++;
	if(usecondcnt==1000000UL)
	{
		 secondcnt++;
	}
	
	EA = 1;
}

*/
/*****************************************************************************
 函 数 名  : void T0_Init(void)
 功能描述  : 定时器0初始化	定时间隔1ms
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   :  陈美书
    修改内容   : 创建
*****************************************************************************/

void T0_Init(void)
{
	  msecondcnt=0;
	  TMOD=0x11;          //16位定时器
    //T0
    TH0=0x00;
    TL0=0x00;
    TR0=0x00;
	  OutTimeFlag=0;
	  EnableTimer=0;	
    TMOD|=0x01;
    TH0=T1MS>>8;        //1ms定时器
    TL0=T1MS;
    ET0=1;              //开启定时器0中断
    EA=1;               //开总中断
    TR0=1;              //开启定时器
}
/*****************************************************************************
 函 数 名  : void T0_ISR_PC(void)    interrupt 1
 功能描述  : 定时器0处理函数，毫秒增加
 输入参数  :	 
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
/*
void T0_ISR_PC(void)    interrupt 1
{
	  u8  i;
    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
	
	
    for(i=0;i<8;i++)
	  {
			if(EnableTimer&(1<<i))
			{
				 TimerTime[i]--;
				 if(TimerTime[i]==0)
				 {
					  OutTimeFlag=OutTimeFlag|(1<<i);
					  EnableTimer&=~(1<<i);
				 					 					 
				 }
			}
		}
		
    EA=1;
}
*/
void T0_ISR_PC(void)    interrupt 1
{
	  u8  i;
    EA=0;
    TH0=T1MS>>8;
    TL0=T1MS;
	
	  msecondcnt++;
	  if(msecondcnt==1000)
		{
			 msecondcnt=0;
		}
	
    for(i=0;i<8;i++)
	  {
			if(EnableTimer&(1<<i))
			{
				 TimerTime[i]--;
				 if(TimerTime[i]==0)
				 {
					  OutTimeFlag=OutTimeFlag|(1<<i);
					  EnableTimer&=~(1<<i);
					 			 					 					 
				 }
			}
			/*
			if(EnablePWM&(1<<i))
			{				
				  PWMTicks[i]++;
				  if(PWMTicks[i]>=PWMTotal[i])
					{
						  PWMTicks[i]=0;
					}
					if(PWMTicks[i]<=PWMLow[i])
					{
						PinOutput(PWMPort[i], PWMPin[i],0);						
					}
          if(PWMTicks[i]>PWMLow[i])
					{
						PinOutput(PWMPort[i], PWMPin[i],1);
					}
			}
			*/
			
		}
		
    EA=1;
}
/*****************************************************************************
 函 数 名  : void StartTimer(u8 ID, u16 nTime)
 功能描述  : 启动一个软定时器，
 输入参数  :	 u8 ID         定时器ID
               u16 nTime     定时ms数

 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
void StartTimer(u8 ID, u16 nTime)
{
	  EA=0;
	  EnableTimer=EnableTimer|(1<<ID);
	  TimerTime[ID]=nTime;
	  OutTimeFlag&=~(1<<ID);
	  EA=1; 
}


/*****************************************************************************
 函 数 名  : void KillTimer(u8 ID)
 功能描述  : 停止一个软定时器，
 输入参数  :	u8 ID  定时器ID
 输出参数  : 
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
void KillTimer(u8 ID)
{
	  EA=0;
	  EnableTimer&=~(1<<(ID));
	  OutTimeFlag&=~(1<<ID);
	  EA=1;
}

/*****************************************************************************
 函 数 名  : u8 GetTimeOutFlag(u8 ID)
 功能描述  : 获得定时器是否超时
 输入参数  :	u8 ID  定时器ID
 输出参数  :  0  未超时    非0  超时
 修改历史  :
  1.日    期   : 2019年9月2日
    作    者   : 陈美书
    修改内容   : 创建
*****************************************************************************/
u8 GetTimeOutFlag(u8 ID)
{
  u8 flag;
	EA=0;
	flag=OutTimeFlag&(1<<ID);
	EA=1;
	return flag;
	
}


/*

void StartPWM(u8 PWMID, u16 VPWMTotal,u16 VPWMLow,u8 Port,u8 Pin )
{
	  EA=0;
	  SetPinOut(Port,Pin);
	  PWMTicks[PWMID]=0;
	  PWMTotal[PWMID]=VPWMTotal;
    PWMLow[PWMID]=VPWMLow;
    PWMPort[PWMID]=Port;
    PWMPin[PWMID]=Pin;	
	  EnablePWM|=(1<<PWMID);
	  EA=1;
}

void StopPWM(u8 PWMID)
{
	  EA=0;
	  EnablePWM&=~(1<<PWMID);	
	  PinOutput(PWMPort[PWMID],PWMPin[PWMID],0);
	  EA=1;
}


void StopAllPWM()
{
	 u8 i;
	 EnablePWM=0;
	 for(i=0;i<8;i++)
	{
		PinOutput(PWMPort[i],PWMPin[i],0);
	}
}



*/


