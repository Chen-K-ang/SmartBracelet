#include <reg52.h>
#include "ls138.h"

void SelectHC573(unsigned char channe1)//选择锁存器
{
	switch(channe1)
	{
		case 4://LED
			P2 = (P2 & 0x1f) | 0x80;//&0001 1111 |1000 0000 100
		break;
		case 5://继电器与蜂鸣器
			P2 = (P2 & 0x1f) | 0xa0;//101
		break;
		case 6://位选码
			P2 = (P2 & 0x1f) | 0xc0;//110
		break;
		case 7://段选码
			P2 = (P2 & 0x1f) | 0xe0;//111
		break;
		case 0:
			P2 = (P2 & 0x1f) | 0x00;//000
		break;
	}
}
