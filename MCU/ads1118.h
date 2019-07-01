#include <msp430.h>
/*
 * ads1118.h
 *
 *  Created on: 2018年5月31日
 *      Author: Remote
 */

#ifndef ADS1118_H_
#define ADS1118_H_
// the first chip
#define      ADS1118_CS         BIT5  //这是配置单片机的管脚，与ads1118上的名字对应
#define      ADS1118_CLK        BIT7
#define      ADS1118_OUT        BIT3
#define      ADS1118_IN         BIT1


#define      ADS1118_Port_OUT   P2OUT  //这个是单片机上管脚开头编号，即P1.0接CS,P1.1接CLK,P1.2接DIN,P1.3接DOUT
#define      ADS1118_Port_DIR   P2DIR
#define      ADS1118_Port_IN    P2IN

#define      ADS1118_CS_OUT     (ADS1118_Port_DIR|=ADS1118_CS)
#define      SET_ADS1118_CS     (ADS1118_Port_OUT|=ADS1118_CS)
#define      CLR_ADS1118_CS     (ADS1118_Port_OUT&=~ADS1118_CS)

#define      ADS1118_CLK_OUT    (ADS1118_Port_DIR|=ADS1118_CLK)
#define      SET_ADS1118_CLK    (ADS1118_Port_OUT|=ADS1118_CLK)
#define      CLR_ADS1118_CLK    (ADS1118_Port_OUT&=~ADS1118_CLK)

#define      ADS1118_OUT_IN     (ADS1118_Port_DIR&=~ADS1118_OUT)
#define      ADS1118_OUT_Val    (ADS1118_Port_IN&ADS1118_OUT)

#define      ADS1118_IN_OUT     (ADS1118_Port_DIR|=ADS1118_IN)
#define      SET_ADS1118_IN     (ADS1118_Port_OUT|=ADS1118_IN)
#define      CLR_ADS1118_IN     (ADS1118_Port_OUT&=~ADS1118_IN)

// the second chip
#define      ADS1118_CS2         BIT5  //这是配置单片机的管脚，与ads1118上的名字对应
#define      ADS1118_CLK2        BIT7
#define      ADS1118_OUT2        BIT1
#define      ADS1118_IN2         BIT3


#define      ADS1118_Port_OUT2   P8OUT  //这个是单片机上管脚开头编号，即P1.0接CS,P1.1接CLK,P1.2接DIN,P1.3接DOUT
#define      ADS1118_Port_DIR2   P8DIR
#define      ADS1118_Port_IN2    P8IN

#define      ADS1118_CS_OUT2     (ADS1118_Port_DIR2|=ADS1118_CS2)
#define      SET_ADS1118_CS2     (ADS1118_Port_OUT2|=ADS1118_CS2)
#define      CLR_ADS1118_CS2     (ADS1118_Port_OUT2&=~ADS1118_CS2)

#define      ADS1118_CLK_OUT2    (ADS1118_Port_DIR2|=ADS1118_CLK2)
#define      SET_ADS1118_CLK2    (ADS1118_Port_OUT2|=ADS1118_CLK2)
#define      CLR_ADS1118_CLK2    (ADS1118_Port_OUT2&=~ADS1118_CLK2)

#define      ADS1118_OUT_IN2     (ADS1118_Port_DIR2&=~ADS1118_OUT2)
#define      ADS1118_OUT_Val2    (ADS1118_Port_IN2&ADS1118_OUT2)

#define      ADS1118_IN_OUT2     (ADS1118_Port_DIR2|=ADS1118_IN2)
#define      SET_ADS1118_IN2     (ADS1118_Port_OUT2|=ADS1118_IN2)
#define      CLR_ADS1118_IN2     (ADS1118_Port_OUT2&=~ADS1118_IN2)

#define      FS          4.096

unsigned int Config_M = 0x42, Config_L = 0xCB;
/*Config_M:0x4* AN0; 0x5* AN1; 0x6* AN2; 0x7* AN3
 * 0x*4 -> 2.048V; 0x*2 -> 4.096V; 0x*0 -> 6.144V
 *
 */
unsigned int Config_Result_M, Config_Result_L;

unsigned char ADS1118_Read(unsigned char);
int ADS1118_Get_U();
void ADS1118_init(void);

unsigned char ADS1118_Read2(unsigned char);
int ADS1118_Get_U2();
void ADS1118_init2(void);

//float ADS1118_Voltage1 = 0, ADS1118_Voltage2 = 0;


unsigned char ADS1118_Read(unsigned char data)   //SPI为全双工通信方式
{
    unsigned char i,temp,Din;
    temp=data;
    for(i=0;i<8;i++)
    {
        Din = Din<<1;
        SET_ADS1118_CLK;
        __delay_cycles(1);
        if(ADS1118_OUT_Val)
            Din |= 0x01;
        if(0x80&temp)
            SET_ADS1118_IN;
        else
            CLR_ADS1118_IN;
        CLR_ADS1118_CLK;
        __delay_cycles(1);
        temp = (temp<<1);
    }
    return Din;
}

unsigned char ADS1118_Read2(unsigned char data)   //SPI为全双工通信方式
{
    unsigned char i,temp,Din;
    temp=data;
    for(i=0;i<8;i++)
    {
        Din = Din<<1;
        SET_ADS1118_CLK2;
        __delay_cycles(1);
        if(ADS1118_OUT_Val2)
            Din |= 0x01;
        if(0x80&temp)
            SET_ADS1118_IN2;
        else
            CLR_ADS1118_IN2;
        CLR_ADS1118_CLK2;
        __delay_cycles(1);
        temp = (temp<<1);
    }
    return Din;
}

void ADS1118_init(void)
{
    ADS1118_CS_OUT;
    ADS1118_CLK_OUT;
    ADS1118_IN_OUT;
    ADS1118_OUT_IN;
    CLR_ADS1118_CS;
    _NOP();
    CLR_ADS1118_CLK;
    _NOP();
    CLR_ADS1118_IN;
    _NOP();
}

void ADS1118_init2(void)
{
    ADS1118_CS_OUT2;
    ADS1118_CLK_OUT2;
    ADS1118_IN_OUT2;
    ADS1118_OUT_IN2;
    CLR_ADS1118_CS2;
    _NOP();
    CLR_ADS1118_CLK2;
    _NOP();
    CLR_ADS1118_IN2;
    _NOP();
}

int ADS1118_Get_U()
{
    CLR_ADS1118_CS;
    unsigned int i=0;
    char Data_REG_H,Data_REG_L;
    int Data_REG;
    while((ADS1118_OUT_Val)&&(i<10000))
        i++;
    Data_REG_H = ADS1118_Read(Config_M);
    __delay_cycles(1);
    Data_REG_L = ADS1118_Read(Config_L);
    Data_REG = (Data_REG_H<<8)+Data_REG_L;
    ADS1118_CS_OUT;
//  Config_Result_M = ADS1118_Read(Config_M);
//  __delay_cycles(1);
//  Config_Result_L = ADS1118_Read(Config_L);
    CLR_ADS1118_IN;
    _NOP();
    if(Data_REG >= 0x8000)
    {
        return 0xFFFF-Data_REG;
        //Data_REG=0xFFFF-Data_REG;//把0xFFFF改成0x10000
        //*ADS1118_Voltage1 = (-1.0)*((Data_REG*0.000125));           //FS/0x8000
    }
    else
    {
        return Data_REG;
        //*ADS1118_Voltage1 = (1.0)*((Data_REG*0.000125));                        //浮点数计算耗时，影响波形
    }
}

int ADS1118_Get_U2()
{
    CLR_ADS1118_CS2;
    unsigned int i=0;
    char Data_REG_H,Data_REG_L;
    int Data_REG;
    while((ADS1118_OUT_Val2)&&(i<10000))
        i++;
    Data_REG_H = ADS1118_Read2(Config_M);
    __delay_cycles(1);
    Data_REG_L = ADS1118_Read2(Config_L);
    Data_REG = (Data_REG_H<<8)+Data_REG_L;
    ADS1118_CS_OUT2;
//  Config_Result_M = ADS1118_Read(Config_M);
//  __delay_cycles(1);
//  Config_Result_L = ADS1118_Read(Config_L);
    CLR_ADS1118_IN2;
    _NOP();
    if(Data_REG >= 0x8000)
    {
        return 0xFFFF-Data_REG;
        //Data_REG=0xFFFF-Data_REG;//把0xFFFF改成0x10000
        //*ADS1118_Voltage1 = (-1.0)*((Data_REG*0.000125));           //FS/0x8000
    }
    else
    {
        return Data_REG;
        //*ADS1118_Voltage1 = (1.0)*((Data_REG*0.000125));                        //浮点数计算耗时，影响波形
    }
}

#endif /* ADS1118_H_ */
