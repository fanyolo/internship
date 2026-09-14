#include "sys.h"
#include "InputData.h"


u16 vp_data;
void dial_ack(u16 number);
void Input_data()
{
	sys_read_vp(0x2000,(u8*)&vp_data,1);
	switch (vp_data)
		{
				case 0x0030:
					dial_ack(vp_data - 0x0030);
					break;
			  case 0x0031:
					dial_ack(vp_data - 0x0030);
					break;
			  case 0x0032:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0033:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0034:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0035:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0036:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0037:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0038:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x0039:
					dial_ack(vp_data - 0x0030);
					break;
				case 0x00F2:
					dial_ack(0x00F2);
					break;
				case 0x00F0:
					dial_ack(0x00F0);
					break;
				default:
					break;	
			}	
}		
		
void dial_ack(u16 number)
{
	u16 value, value_before;
	sys_read_vp(0x2000,(u8*)&value_before,1);
	value = value_before;
  if ((number >= 0) && (number <= 9))  
  {
    value = value * 10 + number;
    if (value < value_before) return;  
  }
  else if (number == 0x00F2)  
  {
    value /= 10;
  }
  else if (number == 0x00F0)  
  {
    value = 0x0000;
  }
	sys_write_vp(0x2000,(u8*)&value,1);
	value = 0x0000;
	sys_write_vp(0x1000,(u8*)&value,1);
}










