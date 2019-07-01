
#include "BUS_FPGA.h"
#include "lcd_serial.h"
//#include "uart.h"

#define KEY_BASE CS0


void KEY4x4_ISR()
{
  unsigned int keyvalue=IORD(KEY_BASE,0);
  switch(keyvalue)
  {
  case 0x11:
	//  uart_TxSting("hello world!");
	  DispString57At(5,50,"0x11");
    break;
  case 0x12:
	//  uart_TxDec(4530,4);
	  DispString57At(5,50,"0x12");
    break;
  case 0x14:
	//  uart_TxHex(0xaa551,5);
	  DispString57At(5,50,"0x14");
    break;
  case 0x18:
	//  uart_TxFloat(23.456,2,3);
	  DispString57At(5,50,"0x18");
    break;
  case 0x21:
	  DispString57At(5,50,"0x21");
    break;
  case 0x22:
	  DispString57At(5,50,"0x22");
    break;
  case 0x24:
	  DispString57At(5,50,"0x24");
    break;
  case 0x28:
	  DispString57At(5,50,"0x28");
    break;
  case 0x41:
	  DispString57At(5,50,"0x41");
    break;
  case 0x42:
	  DispString57At(5,50,"0x42");
    break;
  case 0x44:
	  DispString57At(5,50,"0x44");
    break;
  case 0x48:
	  DispString57At(5,50,"0x48");
    break;
  case 0x81:
	  DispString57At(5,50,"0x81");
    break;
  case 0x82:
	  DispString57At(5,50,"0x82");
    break;
  case 0x84:
	  DispString57At(5,50,"0x84");
    break;
  case 0x88:
	  DispString57At(5,50,"0x88");
    break;
  default:;

  }
}


