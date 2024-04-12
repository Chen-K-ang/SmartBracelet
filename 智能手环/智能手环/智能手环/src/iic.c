
/*-----------------------------------------------
  名称：IIC协议 
  论坛：www.doflye.net
  编写：shifang
  修改：无
  内容：函数是采用软件延时的方法产生SCL脉冲,固对高晶振频率要作 一定的修改....(本例是1us机器
		周期,即晶振频率要小于12MHZ)
------------------------------------------------*/  
  
#include "iic.h"
#include "delay.h"
#include <reg52.h> 


#define ADDR_PCF8591  0x90  //PCF8591器件地址  1001 0000 写，读+1
#define ADDR_24C02    0xA0  //24C02器件地址  1010 0000    写，读+1

sbit SDA=P2^0;
sbit SCL=P2^1;

void Delay_IIC(unsigned char time){
	while(time--);
}

void IIC_Start(void){
	SDA=1;
	SCL=1;
	Delay_IIC(5);
	SDA=0;
	Delay_IIC(5);
	SCL=0;
}

void IIC_Stop(void){
	SDA=0;
	SCL=1;
	Delay_IIC(5);
	SDA=1;
	Delay_IIC(5);
}

void IIC_Ack(unsigned char ackbit){
	if(ackbit) 	SDA=0;
	else		SDA=1;

	Delay_IIC(5);
	SCL=1;
	Delay_IIC(5);
	SCL=0;
	SDA=1;
	Delay_IIC(5);;
}

bit IIC_WaitAck(void){
	SDA=1;
	Delay_IIC(5);
	SCL=1;
	Delay_IIC(5);

	if(SDA){
		SCL=0;
		IIC_Stop();
		return 0;
	}
	else{
		SCL=0;
		return 1;
	}
}

void IIC_SendByte(unsigned char byt){
	unsigned char i;
	for(i=0;i<8;i++){
		if(byt & 0x80)	SDA=1;
		else			SDA=0;
	

		Delay_IIC(5);
		SCL=1;
		byt<<=1;
		Delay_IIC(5);
		SCL=0;
	}
}

unsigned char IIC_RecByte(void){
	unsigned char da, i;
	for(i=0;i<8;i++){
		SCL=1;
		Delay_IIC(5);
		da<<=1;
		if(SDA)	da|=0x01;
		SCL=0;
		Delay_IIC(5);
	}
	return da;
}

/*----------------------------------------------------------------
                    向有子地址器件发送多字节数据函数               
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程,
			从器件地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
----------------------------------------------------------------*/
bit sendBytesTo24C02(unsigned char suba,unsigned char *p,unsigned char no)
{
   	unsigned char i;
  	for(i=0;i<no;i++) { 
		
   	IIC_Start();               	//启动总线
   	IIC_SendByte(ADDR_24C02); 	//发送器件地址
   	IIC_WaitAck();
		IIC_SendByte(suba);         //发送器件子地址
    IIC_WaitAck();
		IIC_SendByte(*p);            //发送数据
 		IIC_WaitAck();
    IIC_Stop();                  //结束总线
	 	
		DelayMs(1);               //必须延时等待芯片内部自动处理数据完毕
	 	p++;
	 	suba++;
    } 
			return(1);
}


/*----------------------------------------------------------------
                    向有子地址器件读取多字节数据函数               
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
----------------------------------------------------------------*/
bit recBytesFrom24C02(unsigned char suba,unsigned char *p,unsigned char no)
{
	unsigned char i;
   	IIC_Start();               //启动总线			 
   	IIC_SendByte(ADDR_24C02);             //发送器件地址
	IIC_WaitAck();   	
	IIC_SendByte(suba);            //发送器件子地址
	IIC_WaitAck();
   	IIC_Start();
   	IIC_SendByte(ADDR_24C02+1);				//读动作 1
	IIC_WaitAck();
  	
	for(i=0;i<no-1;i++) { 
     	*p=IIC_RecByte();              //发送数据
      	IIC_Ack(1);                //发送就答位 
     	p++;
    } 
   	*p=IIC_RecByte();
    IIC_Ack(0);                 //发送非应位
    IIC_Stop();                    //结束总线
  	return(1);
}


unsigned char Read_PCF8591(unsigned char com ){
	unsigned char dat;

	IIC_Start();								
	IIC_SendByte(ADDR_PCF8591); 				
	IIC_WaitAck();  			 				
	IIC_SendByte(com); 					
	IIC_WaitAck();  							
	IIC_Stop(); 							

	IIC_Start();									
	IIC_SendByte(ADDR_PCF8591+1); 					
	IIC_WaitAck(); 										
	dat = IIC_RecByte(); 			
	IIC_Ack(0); 								
	IIC_Stop(); 									

	return dat;
}





