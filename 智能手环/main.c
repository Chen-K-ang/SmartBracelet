#include <reg51.h>
#include <intrins.h>
#include "iic.h"
#include "smg.h"
extern unsigned char index;
//全局变量定义
unsigned int KeyCode,S7Count,S7Lock,S6Count,S6Lock,S5Count,S5Lock,S4Count,S4Lock;
unsigned char step,TimeFlag;
unsigned int TimeCount,LedStep,LedCount,TimeCount2;
unsigned int LightTimeCount;
unsigned int Led1Flag,Led2Flag,RlyFlag,SaveFlag;
unsigned int AD_Value,HeartBeat;
unsigned int Hour,Minute,Second;
unsigned char dispBuf[8];
unsigned char buf[4];
unsigned char sciBuf[15];
unsigned char HourCount,MinuteCount,SecondCount;
//定义按键位操作
sbit S7 = P1^0;
sbit S6 = P1^1;
sbit S5 = P1^2;
sbit S4 = P1^3;
//按键扫描函数
void KeyScan(void)
{
  if(S7 == 1)            //按键S7松开，则接除锁定
	{
	   S7Count = 0;      
		 S7Lock = 0;
	}
	else if(S7Lock == 0) //按键S7没被锁定，然后有按下
	{
	   S7Count++;
		 if(S7Count == 2) //消抖
		 {
			  S7Count = 0;//确认按键S7按下，送键值
		    S7Lock = 1;   
			  KeyCode = 1;
		 }
	}
	if(S6 == 1)
	{
	   S6Count = 0;
		 S6Lock = 0;
	}
	else if(S6Lock == 0)
	{
	   S6Count++;
		 if(S6Count == 2)
		 {
			  S6Count = 0;
		    S6Lock = 1;
			  KeyCode = 2;
		 }
	}
	if(S5 == 1)
	{
	   S5Count = 0;
		 S5Lock = 0;
	}
	else if(S5Lock == 0)
	{
	   S5Count++;
		 if(S5Count == 2)
		 {
			  S5Count = 0;
		    S5Lock = 1;
			  KeyCode = 3;
		 }
	}
	if(S4 == 1)
	{
	   S4Count = 0;
		 S4Lock = 0;
	}
	else if(S4Lock == 0)
	{
	   S4Count++;
		 if(S4Count == 2)
		 {
			  S4Count = 0;
		    S4Lock = 1;
			  KeyCode = 4;
		 }
	}
}
//定时器1初始化，5ms的中断周期
void TimerInit(void)
{
    TMOD |= 0x01;                //定时器0  16位模式
    TH0 = (65535 - 5000)>>8;	 //送初值
	  TL0 = (65535 - 5000)&0xff;  	
	  ET0 = 1;                   //使能中断，并开启定时器0
		EA = 1;                  
    TR0 = 1;

}

//串口初始化
void UartInit(void)
{
		SCON = 0x50;             //8位数据模式
		TMOD |= 0x20;            //配置定时器0位 8位自动模式，串口波特率位9600，数据格式位1-8-1-N，
		PCON = 0x00;
		TH1  = 0xfd;
		TL1  = 0xfd;
		IP   = 0x01;
		TR1  = 1;
}

//系统初始化，主要是对相关变量的初始化
void SystemInit(void)
{
    P0 = 0xff;
    Led1Flag = 1;
    Led2Flag = 0;
    Hour = 12;
    Minute = 0;
    Second = 0;
    LedStep = 0;
    LedCount = 0;
    step = 0;
}

void UART_SendDat(unsigned char dat)
{
    SBUF = dat;
	  while (TI == 0);
	  TI = 0;					
}
//串口字符串发送函数，用来发送字符串
void UartSendString(unsigned char *p)
{
    while(*p != '\0')        //发送的字符串未结束
    {
        SBUF = *p;           //发送当前字符
        while (TI == 0);     //等待发送完成
        TI = 0;
        p++;                 //指针指向下一个字符
    }
}


void main(void)
{ 
    SystemInit();        //初始化
    UartInit();
    TimerInit();         
    step = 4;
	  while(1)
		{
       KeyScan();	
       if(LightTimeCount > 100)   //500ms
       {
            LightTimeCount = 0;		
            AD_Value = Read_PCF8591(0);	                  //读取PCF8591
            HeartBeat = (unsigned long)AD_Value / 256 * 150;
            if((HeartBeat < 60)||(HeartBeat > 100))       
            {
               SaveFlag = 1;                              //超过报警值             
               buf[0] = Hour;
               buf[1] = Minute;
               buf[2] = Second;
               buf[3] = HeartBeat;
               sendBytesTo24C02(0x10,buf,4);
            }       
         }
			 
				 //开关机键S7
         if(KeyCode == 1)
         {
            KeyCode = 0;        
            if(step != 4)    
            {
               step = 4;            
            }
            else
            {
               SystemInit();              
               step = 0;
            }
         }   
				 
         //关机时			 
         if(step == 4)
         {
            Led1Flag = 0;
            dispBuf[0] = 10;	
            dispBuf[1] = 10;
            dispBuf[2] = 10;	
            dispBuf[3] = 10;
            dispBuf[4] = 10;
            dispBuf[5] = 10; 
            dispBuf[6] = 10;
            dispBuf[7] = 10;	              
         }       

					//时间显示状态				 
         else if(step == 0)         
         {
            if(KeyCode == 2)
            {
               index = 0;
               KeyCode = 0;     
               step = 1;						
            }
            dispBuf[0] = Hour/10;
            dispBuf[1] = Hour%10;
            dispBuf[2] = 11;	
            dispBuf[3] = Minute/10;
            dispBuf[4] = Minute%10;
            dispBuf[5] = 11; 
            dispBuf[6] = Second/10;
            dispBuf[7] = Second%10;	    	 				
         }
				 //心跳显示状态
         else if(step == 1)         
         {
            if((HeartBeat < 60)||(HeartBeat > 100))
            {
               Led2Flag = 1;        //心跳异常
            }
            else
            {
               LedStep = 0;
               LedCount = 0;
               Led2Flag = 0;
            }  
            if(KeyCode == 2)        //切换状态
            {
               index = 0;
               KeyCode = 0; 
               LedStep = 0;
               LedCount = 0;
               Led2Flag = 0;            
               step = 0;						
            }          
            dispBuf[0] = 14;	   //数据
            dispBuf[1] = 15;
            dispBuf[2] = 16;	
            dispBuf[3] = 11;
            dispBuf[4] = 11;
            dispBuf[5] = HeartBeat/100;
            dispBuf[6] = HeartBeat%100/10;
            dispBuf[7] = HeartBeat%10;          
         }
         if((KeyCode == 3)&&(step != 4))    //按键S5按键
         {
            KeyCode = 0;
            if(SaveFlag == 1)               //EEPROM中存有异常心跳数据
            {
                recBytesFrom24C02(0x10,buf,4);   //读取EEPROM的数据记录
                UartSendString("2021-08-02 ");
                sciBuf[0] = buf[0]/10 + '0';
                sciBuf[1] = buf[0]%10 + '0'; 
                sciBuf[2] = ':';   
                sciBuf[3] = buf[1]/10 + '0';
                sciBuf[4] = buf[1]%10 + '0'; 
                sciBuf[5] = ':';
                sciBuf[6] = buf[2]/10 + '0'; 
                sciBuf[7] = buf[2]%10 + '0';
                sciBuf[8] = ' ';
                if(buf[3] > 120)
                {
                   sciBuf[9] =  'H';            
                }
                else if(buf[3] < 60)
                {
                   sciBuf[9] =  'L';             
                }
                sciBuf[10] = buf[3]/100 + '0';
                sciBuf[11] = buf[3]%100/10 + '0';     
                sciBuf[12] = buf[3]%10 + '0'; 
                sciBuf[13] = '\r';                     
                sciBuf[14] = '\0';             
                UartSendString(sciBuf);                
            }      
         }
          if(KeyCode == 4)          //按键S4按下 
          {
             KeyCode = 0;
             if(step != 5)  
             {
                step = 5;
                HourCount = 0;      //定时的初值为 00-00-00
                MinuteCount = 0;
                SecondCount = 0;
                TimeFlag = 0;
             }
             else
             {
                if(TimeFlag == 0)    //暂停 启动
                {
                   TimeFlag = 1;
                }
                else
                {
                   TimeFlag = 0;                
                }
             }               
          }
          if(step == 5)             //定时状态
          {
             if(KeyCode == 2)       //切换为时间定时状态 
             {
                KeyCode = 0;
                step = 0;
                TimeFlag = 0;
             }
            dispBuf[0] = HourCount/10;
            dispBuf[1] = HourCount%10;
            dispBuf[2] = 11;	
            dispBuf[3] = MinuteCount/10;
            dispBuf[4] = MinuteCount%10;
            dispBuf[5] = 11; 
            dispBuf[6] = SecondCount/10;
            dispBuf[7] = SecondCount%10;	             
          }            
      }         
}
//定时器0中断服务函数，周期为5ms
void  timer1(void) interrupt 1
{
    TH0 = (65535 - 5000)>>8;	
	  TL0 = (65535 - 5000)&0xff;
    TimeCount++;
  	LightTimeCount++;
    if(TimeCount >= 200)      //5ms*200 = 1s
    {                         //时间显示状态下，时分秒数据的定时跟新
       TimeCount = 0;
       Second++;
       if(Second >= 60)
       {
          Second = 0;
          Minute++;
          if(Minute >= 60)
          {
             Minute = 0;
             Hour++;
             if(Hour >= 24)
             {
                 Hour = 0;
             }
          }
       }
    }
    if(TimeFlag == 1)
    {                           //定时状态下，时分秒数据的定时跟新
        TimeCount2++;
        if(TimeCount2 >= 200)
        {
           TimeCount2 = 0;
           SecondCount++;
           if(SecondCount >= 60)
           {
              SecondCount = 0;
              MinuteCount++;
              if(MinuteCount >= 60)
              {
                 MinuteCount = 0;
                 HourCount++;
                 if(HourCount >= 24)
                 {
                     HourCount = 0;
                 }
              }
           }
        }
    }
	  showLed();                  //数码管动态显示
		P2 = ((P2 & 0x1f) | 0xa0);   //LED灯显示
		P2 &= 0x1f;
    if(Led2Flag == 1)              //流水灯告警显示
    {
       LedCount++;
       if(LedCount >= 100)         //300ms的流水周期
       {
          LedCount = 0;
          LedStep++;
          if(LedStep > 2)          //总共3个灯流水，还有1个开机灯则保持常亮
          {
             LedStep = 0;
          }            
       }
       if(LedStep == 0)
       {
          P0 = 0xfc;               //LED0,LED1亮，LED2,LDE3灭
       }
       else if(LedStep == 1)
       {
          P0 = 0xfa;               //LED0,LED2亮，LED1,LDE3灭   
       }
       else if(LedStep == 2)
       {
          P0 = 0xf6;               //LED0,LED3亮，LED1,LDE2灭
       }
    }
    else
    {
        P0 = 0xfe;  
    }
		if(Led1Flag == 0)
		{
   		 P0 = 0xff;                //关机状态，熄灭所有灯
		}

	  P2 = ((P2 & 0x1f) | 0x80);
	  P2 &= 0x1f;
}