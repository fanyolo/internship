#include "sys.h"
#include "nor_flash.h"
#include "timer.h"
#include "InputData.h"

#define Logo_In_VP 0X1000
#define Reset_Logoin_VP 0X1010
#define Reset_confirm_VP 0X1014
#define NOR_FLASH_DATA_ADDR		0    //地址必须是偶数


u32 Initial_password,password,Reset_password,reconfirmpassword,NOR_FLASH_DATA;
u16 Confirm_key,pageID;
u8 i;

	
void Next_page(u16 Page_Id);
void clr_vp_data(u16 vp,u16 len);
void main(void)
{  	 
		sys_init();//系统初始化
		norflash_read(NOR_FLASH_DATA_ADDR,(u8*)&Initial_password,2);//
		while(1)
		{
/*********************密码登陆*************************/
			Input_data();
			sys_read_vp(Logo_In_VP,(u8*)&password,2);//读取密码
			if(Initial_password==password)
				{
					Next_page(1);//密码验证成功
					clr_vp_data(Logo_In_VP,2);		
				}				
			else
				{
					if(password==0)//排除未输入的情况
						{
//							continue;
						}
						else
						{
							Next_page(2);//密码错误
							clr_vp_data(Logo_In_VP,2);//break;
						}
					}
							
/******************重置密码*****************/
			sys_read_vp(Reset_Logoin_VP,(u8*)&Reset_password,2);//读取重置密码
			sys_delay_ms(2);
			sys_read_vp(Reset_confirm_VP,(u8*)&reconfirmpassword,2);//读取确认密码
			sys_delay_ms(2);
			sys_read_vp(0x1020,(u8*)&Confirm_key,1);//读取确认密码
			if(Confirm_key==1)
			{
				if((Reset_password==reconfirmpassword)&&(Reset_password!=0)&&(reconfirmpassword!=0))
				{
					Next_page(6);//密码一致,切换到第6页,
					clr_vp_data(Reset_Logoin_VP,20);
					Initial_password=reconfirmpassword;
					norflash_write(NOR_FLASH_DATA_ADDR,(u8*)&reconfirmpassword,2);
				}				
				if(Reset_password!=reconfirmpassword)
				{
					Next_page(5);//密码不一致，切换到第5页
					clr_vp_data(Reset_Logoin_VP,20);
				}				
			}
	
		}		 
}

/*******************切页功能子函数*****************/	
void Next_page(u16 Page_Id)//切页
{
	u16 cmd[2];
	cmd[0]=0x5A01;cmd[1]=Page_Id;
	sys_write_vp(0x0084,(u8*) cmd,2);
}

/*******************清除变量地址数据功能****************/	
void clr_vp_data(u16 vp,u16 len)//清除2K字的变量数据
{
	u16 buf[1024];
	for(i=0;i<len;i++)
	{buf[i]=0;}
	sys_write_vp(vp,(u8*) buf,len);
}










