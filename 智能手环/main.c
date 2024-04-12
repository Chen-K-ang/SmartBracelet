#include <reg51.h>
#include <intrins.h>
#include "iic.h"
#include "smg.h"
extern unsigned char index;
//ȫ�ֱ�������
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
//���尴��λ����
sbit S7 = P1^0;
sbit S6 = P1^1;
sbit S5 = P1^2;
sbit S4 = P1^3;
//����ɨ�躯��
void KeyScan(void)
{
  if(S7 == 1)            //����S7�ɿ�����ӳ�����
	{
	   S7Count = 0;      
		 S7Lock = 0;
	}
	else if(S7Lock == 0) //����S7û��������Ȼ���а���
	{
	   S7Count++;
		 if(S7Count == 2) //����
		 {
			  S7Count = 0;//ȷ�ϰ���S7���£��ͼ�ֵ
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
//��ʱ��1��ʼ����5ms���ж�����
void TimerInit(void)
{
    TMOD |= 0x01;                //��ʱ��0  16λģʽ
    TH0 = (65535 - 5000)>>8;	 //�ͳ�ֵ
	  TL0 = (65535 - 5000)&0xff;  	
	  ET0 = 1;                   //ʹ���жϣ���������ʱ��0
		EA = 1;                  
    TR0 = 1;

}

//���ڳ�ʼ��
void UartInit(void)
{
		SCON = 0x50;             //8λ����ģʽ
		TMOD |= 0x20;            //���ö�ʱ��0λ 8λ�Զ�ģʽ�����ڲ�����λ9600�����ݸ�ʽλ1-8-1-N��
		PCON = 0x00;
		TH1  = 0xfd;
		TL1  = 0xfd;
		IP   = 0x01;
		TR1  = 1;
}

//ϵͳ��ʼ������Ҫ�Ƕ���ر����ĳ�ʼ��
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
//�����ַ������ͺ��������������ַ���
void UartSendString(unsigned char *p)
{
    while(*p != '\0')        //���͵��ַ���δ����
    {
        SBUF = *p;           //���͵�ǰ�ַ�
        while (TI == 0);     //�ȴ��������
        TI = 0;
        p++;                 //ָ��ָ����һ���ַ�
    }
}


void main(void)
{ 
    SystemInit();        //��ʼ��
    UartInit();
    TimerInit();         
    step = 4;
	  while(1)
		{
       KeyScan();	
       if(LightTimeCount > 100)   //500ms
       {
            LightTimeCount = 0;		
            AD_Value = Read_PCF8591(0);	                  //��ȡPCF8591
            HeartBeat = (unsigned long)AD_Value / 256 * 150;
            if((HeartBeat < 60)||(HeartBeat > 100))       
            {
               SaveFlag = 1;                              //��������ֵ             
               buf[0] = Hour;
               buf[1] = Minute;
               buf[2] = Second;
               buf[3] = HeartBeat;
               sendBytesTo24C02(0x10,buf,4);
            }       
         }
			 
				 //���ػ���S7
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
				 
         //�ػ�ʱ			 
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

					//ʱ����ʾ״̬				 
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
				 //������ʾ״̬
         else if(step == 1)         
         {
            if((HeartBeat < 60)||(HeartBeat > 100))
            {
               Led2Flag = 1;        //�����쳣
            }
            else
            {
               LedStep = 0;
               LedCount = 0;
               Led2Flag = 0;
            }  
            if(KeyCode == 2)        //�л�״̬
            {
               index = 0;
               KeyCode = 0; 
               LedStep = 0;
               LedCount = 0;
               Led2Flag = 0;            
               step = 0;						
            }          
            dispBuf[0] = 14;	   //����
            dispBuf[1] = 15;
            dispBuf[2] = 16;	
            dispBuf[3] = 11;
            dispBuf[4] = 11;
            dispBuf[5] = HeartBeat/100;
            dispBuf[6] = HeartBeat%100/10;
            dispBuf[7] = HeartBeat%10;          
         }
         if((KeyCode == 3)&&(step != 4))    //����S5����
         {
            KeyCode = 0;
            if(SaveFlag == 1)               //EEPROM�д����쳣��������
            {
                recBytesFrom24C02(0x10,buf,4);   //��ȡEEPROM�����ݼ�¼
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
          if(KeyCode == 4)          //����S4���� 
          {
             KeyCode = 0;
             if(step != 5)  
             {
                step = 5;
                HourCount = 0;      //��ʱ�ĳ�ֵΪ 00-00-00
                MinuteCount = 0;
                SecondCount = 0;
                TimeFlag = 0;
             }
             else
             {
                if(TimeFlag == 0)    //��ͣ ����
                {
                   TimeFlag = 1;
                }
                else
                {
                   TimeFlag = 0;                
                }
             }               
          }
          if(step == 5)             //��ʱ״̬
          {
             if(KeyCode == 2)       //�л�Ϊʱ�䶨ʱ״̬ 
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
//��ʱ��0�жϷ�����������Ϊ5ms
void  timer1(void) interrupt 1
{
    TH0 = (65535 - 5000)>>8;	
	  TL0 = (65535 - 5000)&0xff;
    TimeCount++;
  	LightTimeCount++;
    if(TimeCount >= 200)      //5ms*200 = 1s
    {                         //ʱ����ʾ״̬�£�ʱ�������ݵĶ�ʱ����
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
    {                           //��ʱ״̬�£�ʱ�������ݵĶ�ʱ����
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
	  showLed();                  //����ܶ�̬��ʾ
		P2 = ((P2 & 0x1f) | 0xa0);   //LED����ʾ
		P2 &= 0x1f;
    if(Led2Flag == 1)              //��ˮ�Ƹ澯��ʾ
    {
       LedCount++;
       if(LedCount >= 100)         //300ms����ˮ����
       {
          LedCount = 0;
          LedStep++;
          if(LedStep > 2)          //�ܹ�3������ˮ������1���������򱣳ֳ���
          {
             LedStep = 0;
          }            
       }
       if(LedStep == 0)
       {
          P0 = 0xfc;               //LED0,LED1����LED2,LDE3��
       }
       else if(LedStep == 1)
       {
          P0 = 0xfa;               //LED0,LED2����LED1,LDE3��   
       }
       else if(LedStep == 2)
       {
          P0 = 0xf6;               //LED0,LED3����LED1,LDE2��
       }
    }
    else
    {
        P0 = 0xfe;  
    }
		if(Led1Flag == 0)
		{
   		 P0 = 0xff;                //�ػ�״̬��Ϩ�����е�
		}

	  P2 = ((P2 & 0x1f) | 0x80);
	  P2 &= 0x1f;
}