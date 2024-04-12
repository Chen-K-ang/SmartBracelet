#include <reg52.h>
#include "smg.h"


#define uchar unsigned char
#define uint unsigned int 
extern unsigned char mode;	
extern unsigned char ledtime;
extern unsigned int dispDS1,dispDS2,step,PowerOnFlag;
extern unsigned char dispBuf[8];
//共阳极数码管
			          //    0   1    2    3    4     5    6   7    8    9    灭   -     .    全亮
uchar code zixing[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0x7f,0x00,0x89,0x86,0x88};
uchar code weix[8]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uchar index;
unsigned int dispTmp1;
unsigned char dotFlag;

void smg_displayLogic(void)
{
    if(index > 7)
    {
       index = 0;
    }  
    dispTmp1 = dispBuf[index];	
}

void showLed()
{
  smg_displayLogic();
	P0 = weix[index];
	P2 = ((P2 & 0x1f) | 0xc0);//110 
	P2 &= 0x1f;//P2前三位清0 不选择锁存器
	if(dotFlag)
	{
		 P0 = zixing[dispTmp1] & 0x7f;
	}
	else
  {
		 P0 = zixing[dispTmp1];
	}
	P2 = ((P2 & 0x1f)|0xe0);//111
	P2 &= 0x1f;//P2前三位清0 不选择锁存器
	index++;
}


