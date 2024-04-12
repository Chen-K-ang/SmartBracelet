#ifndef __I2C_H__
#define __I2C_H__  
          
bit sendBytesTo24C02(unsigned char suba,unsigned char *p,unsigned char no);
bit recBytesFrom24C02(unsigned char suba,unsigned char *p,unsigned char no);
unsigned char Read_PCF8591(unsigned char com );
#endif





