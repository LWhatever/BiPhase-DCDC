/*
 * lcd_serial.h
 *
 *  Created on: 2013-8-16
 *      Author: Administrator
 */

#ifndef LCD_SERIAL_H_
#define LCD_SERIAL_H_


#include "ASCII.h"

#define SET_CS  (P8OUT|=BIT7)
#define CLR_CS  (P8OUT&=~BIT7)
#define SET_A0  (P8OUT|=BIT6)
#define CLR_A0  (P8OUT&=~BIT6)

const long long numtab[]={
  1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,10000000000};

/**************************************************************
 * 						函数申明
 *
 **************************************************************/
unsigned char SPIB1_Txdat(unsigned char dat);
void SPIB1_Init(void);
void wr_dat(unsigned char dat);
void wr_cmd(unsigned char cmd);
void lcd_init();
void lcd_clear(void);

void display_graphic_8x16(unsigned int page,unsigned int column,const unsigned char *dp);

/**************************************************************
 * 						SPI_transmit
 *
 **************************************************************/
unsigned char SPIB1_Txdat(unsigned char dat)
{
	while (!(UCB1IFG&UCTXIFG));       // USCI_A0 TX buffer ready?
	UCB1TXBUF = dat;                    // Transmit first character
	while(!(UCB1IFG&UCRXIFG));
//	UCB1IFG&=~UCRXIFG;
	return UCB1RXBUF;
}
void SPIB1_Init(void)
{
	P8SEL |= BIT4+BIT5;                  	// Assign P2.0 to UCB0CLK and...
	P8DIR |= BIT4+BIT5;                 	// P2.1 UCB0SOMI P2.2 UCB0SIMO

	UCB1CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCB1CTL0 |= UCMST+UCSYNC+UCCKPL+UCMSB;    // 3-pin, 8-bit SPI master
	                                          // Clock polarity high, MSB
	UCB1CTL1 |= UCSSEL_2;                     // SMCLK
	UCB1BR0 = 0x04;                           // SMCLK/4 = 8Mhz
	UCB1BR1 = 0;                              //
//  UCB1MCTL = 0;                             // No modulation
	UCB1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
//  UCA0IE = UCRXIE ;                         // Enable USCI_A0 RX,TX interrupt
}

/**************************************************************
 * 						底层函数
 *
 **************************************************************/
/*=======写指令========*/
void wr_cmd(unsigned char cmd)
{
	CLR_CS;
	CLR_A0;
	SPIB1_Txdat(cmd);
	SET_CS;
}

/*--------写数据------------*/
void wr_dat(unsigned char dat)
{
	CLR_CS;
	SET_A0;
	SPIB1_Txdat(dat);
	SET_CS;
}

/**************************************************************
 * 						初始化
 *
 **************************************************************/
void lcd_clear(void)
{
	unsigned char i,j;
	for(i=0;i<9;i++)
	{
		wr_cmd(0xb0+i);
		wr_cmd(0x10);
		wr_cmd(0x00);
		for(j=0;j<132;j++)
		{
			wr_dat(0x00);
		}
	}
}

void Lcd_Init()
{
	SPIB1_Init();
	P8DIR|=BIT6+BIT7;
	SET_CS;
	wr_cmd(0xe2);    /*软复位*/
	wr_cmd(0x2c);    /*升压步聚 1*/
	wr_cmd(0x2e);    /*升压步聚 2*/
	wr_cmd(0x2f);    /*升压步聚 3*/
	wr_cmd(0x23);    /*粗调对比度，可设置范围 20～27*/
	wr_cmd(0x81);    /*微调对比度*/
	wr_cmd(0x28);    /*微调对比度的值，可设置范围 0～63*/
	wr_cmd(0xa2);    /*1/9 偏压比（bias）*/
	wr_cmd(0xc8);    /*行扫描顺序：从上到下*/
	wr_cmd(0xa0);    /*列扫描顺序：从左到右*/
	wr_cmd(0xaf);    /*开显示*/
	lcd_clear();
}
/**************************************************************
 * 						显示
 *
 **************************************************************/

void lcd_addr(unsigned char page,unsigned char column)
{
	column=column;
	wr_cmd(0xb0+page); /*设置页地址*/
	wr_cmd(0x10+(column>>4&0x0f)); /*设置列地址的高4 位*/
	wr_cmd(column&0x0f); /*设置列地址的低4 位*/
}

/*显示8x16 点阵图像、ASCII, 或8x16 点阵的自造字符、其他图标*/
void disp_graph_8x16(unsigned char page,unsigned char column,const unsigned char *dp)
{
	unsigned char i,j;
	for(j=0;j<2;j++)
	{
		lcd_addr(page+j,column);
		for (i=0;i<8;i++)
		{
			wr_dat(*dp); /*写数据到LCD,每写完一个8 位的数据后列地址自动加1*/
			dp++;
		}
	}
}
/*显示 16x16 点阵图像、汉字、生僻字或 16x16 点阵的其他图标*/
void disp_graph_16x16(unsigned char page,unsigned char column,const unsigned char *dp)
{
	unsigned char i,j;
	for(j=0;j<2;j++)
	{
		lcd_addr(page+j,column);
		for (i=0;i<16;i++)
		{
			wr_dat(*dp);     /*写数据到 LCD,每写完一个 8 位的数据后列地址自动加 1*/
			dp++;
		}
	}
}
/*显示 5*7 点阵图像、ASCII, 或 5x7 点阵的自造字符、其他图标*/
void disp_graph_5x7(unsigned char page,unsigned char column,const unsigned char *dp)
{
	unsigned char i;
 	lcd_addr(page,column);
 	for (i=0;i<5;i++)
 	{
 		wr_dat(*dp);
 		dp++;
 	}
}
/**************************************************************
 * 						高层显示
 *
 **************************************************************/
/*=======8*16========*/
void DispStringAt(unsigned char x,unsigned char y,char *str)
{
  unsigned char i;
  for(i=0;;i++)
  {
    if(str[i]==0)
      break;
    disp_graph_8x16(x,y+i*8,ASCII8_16[str[i]-' ']);
  }
}

void DispDecAt(unsigned char x,unsigned char y,int dat,unsigned char len)
{
	char str[12];
	unsigned char i,dl;
	str[len]=0;
	for(i=0;i<len;i++)
	{
		dl=dat%10;
		dat/=10;
		str[len-i-1]=dl+'0';
	}
	DispStringAt(x,y,str);
}
void DispHexAt(unsigned char x,unsigned char y,long dat,unsigned char len)
{
	unsigned char i,dl;
	char str[12];
	for(i=0;i<len;i++)
	{
		dl=dat&0xf;
		dat=dat>>4;
		if(dl<10)
			str[len-i-1]=dl+'0';
		else
			str[len-i-1]=dl-10+'a';
	}
	DispStringAt(x,y,str);
}
void DispFloatAt(unsigned char x,unsigned char y,float dat,unsigned char len1,unsigned char len2 )
{
	int dat1,dat2;
	dat1=(int)dat;
	dat2=(int)((dat-dat1)*numtab[len2]);
	DispDecAt(x,y,dat1,len1);
	DispStringAt(x,y+8*len1,".");
	DispDecAt(x,y+8*(len1+1),dat2,len2);
}
/*=======5*7========*/
void DispString57At(unsigned char x,unsigned char y,char *str)
{
	  unsigned char i;
	  for(i=0;;i++)
	  {
	    if(str[i]==0)
	      break;
	    disp_graph_5x7(x,y+i*6,ASCII5_7[str[i]-' ']);
	  }
}
void DispDec57At(unsigned char x,unsigned char y,int dat,unsigned char len)
{
	char str[12];
	unsigned char i,dl;
	str[len]=0;
	for(i=0;i<len;i++)
	{
		dl=dat%10;
		dat/=10;
		str[len-i-1]=dl+'0';
	}
	DispString57At(x,y,str);
}
void DispHex57At(unsigned char x,unsigned char y,long dat,unsigned char len)
{
	unsigned char i,dl;
	char str[12];
	str[len]=0;
	for(i=0;i<len;i++)
	{
		dl=dat&0xf;
		dat=dat>>4;
		if(dl<10)
			str[len-i-1]=dl+'0';
		else
			str[len-i-1]=dl-10+'a';
	}
	DispString57At(x,y,str);
}
//DispFloat57At(5,3*12,data3,6,2);
void DispFloat57At(unsigned char x,unsigned char y,float dat,unsigned char len1,unsigned char len2 )
{
	int dat1,dat2;
	dat1=(int)dat;
	dat2=(int)((dat-dat1)*numtab[len2]);
	DispDec57At(x,y,dat1,len1);
	DispString57At(x,y+6*len1,".");
	DispDec57At(x,y+6*(len1+1),dat2,len2);
}
void DispDouble57At(unsigned char x,unsigned char y,double dat,unsigned char len1,unsigned char len2 )
{
	int dat1,dat2;
	dat1=(int)dat;
	dat2=(int)((dat-dat1)*numtab[len2]);
	DispDec57At(x,y,dat1,len1);
	DispString57At(x,y+6*len1,".");
	DispDec57At(x,y+6*(len1+1),dat2,len2);
}
//显示长度较大的浮点型小数
void DispSmall57At(unsigned char x,unsigned char y,float dat,unsigned char len1,unsigned char len2 )
{
	int dat1;
        long dat2;
	dat1=(int)dat;
	dat2=(long)((dat-dat1)*numtab[len2]);
	DispDec57At(x,y,dat1,len1);
	DispString57At(x,y+6*len1,".");
	char str[12];
	unsigned char i,dl;
	str[len2]=0;
	for(i=0;i<len2;i++)
	{
		dl=dat2%10;
		dat2/=10;
		str[len2-i-1]=dl+'0';
	}
	DispString57At(x,y+6*(len1+1),str);
}
//显示一张全屏图片
void DispImg12864(unsigned char graphic[]){
	unsigned char *address;		//声明数据指针地址
	int i,j;
	address = graphic;
	for(i = 0;i < 8;i ++){
		CLR_CS;	//	屏幕片选清零
		wr_cmd(0xb0 + i);	//set page address
		wr_cmd(0x10);
		wr_cmd(0x00);
		for(j = 0;j < 128;j ++){
			wr_dat(*address);
			address ++;
		}
	}
}
#endif /* LCD_SERIAL_H_ */
