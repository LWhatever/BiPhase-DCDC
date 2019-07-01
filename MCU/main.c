/**
 * [PFC]
 * @Author    [Li Zhongya]
 * @DateTime  2019-04-23T14:49:45+0800
 * @Copyright [Wuhan University]
 * @Version   [1.0]
 */
#include "msp430.h"
#include "lcd_serial.h"
#include "BUS_FPGA.h"
#include "pid_delta.h"
#include "ads1118.h"
#include "math.h"

unsigned int key_value;
unsigned int ch4, ch5, Sample_num;
double ch4_r, ch5_r;
int V_DC1;
int V_DC2;
int Max_V,Max_I;
long long int V_Rms0 = 0, V_Rms1 = 0, V_Rms2 = 0;
double V_Rms = 0;
long long int I_Rms0 = 0, I_Rms1 = 0, I_Rms2 = 0;
double I_Rms = 0;
long long int P0 = 0, P1 = 0, P2 = 0;
double Power = 0, PF = 0;
int V_Now = 0,I_Now = 0;
unsigned int state = 0, boost_state = 0;
unsigned int spwm_en = 0,pwm_en = 0;


double dealtV = 1.001;
unsigned int dealtV_WR = ((int)(1.001 * 2048)) | 0x2000;
PID_DELTA pid1; //PID structure
double pwm_cycle_test = 1800;
double P = 2.5, I = 2.0, D = 0.33;

int V_DC1_goal = 18140;         //36/16 = 2.25 -> 18130; 45/16 = 2.8125V -> 22640
int V_DC2_goal = 18090;
unsigned int delay = 0;
unsigned int tri_div = 16, tri_div_WR = 16 | 0x4000;
unsigned int KW = 32, KW_WR = 32 | 0x6000;
unsigned int pwm_cycle = 1800, pwm_cycle_WR = 1800 | 0x8000;

void show(double data, unsigned char x, unsigned char y, unsigned char len1, unsigned char len2)
{
    unsigned char y1 = y-1;
    if(data<0)
    {
        DispString57At(x,y1*4,"-");
        DispFloat57At(x,y*4,-data,len1,len2);
    }
    else
    {
        DispString57At(x,y1*4," ");
        DispFloat57At(x,y*4,data,len1,len2);
    }
}

void GPIO_Init()
{
    P5REN |= BIT3;
//    P5REN |= BIT2;
    P5DIR |= BIT3;
    P5OUT &= ~BIT3;
//    P5DIR |= BIT2;
//    P5OUT &= ~BIT2;
}

void Timer_Init()
{
    TA0CCR0 = 224;
    TA0CTL |= MC_1 + TASSEL_2 + TACLR;
    TA0CCTL0 = CCIE;

    TA1CCR0 = 22400;
    TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
    TA1CCTL1 = CCIE;
}

void FPGA_Init()
{
    IOWR(0x10,0,delay);
    IOWR(0x10,0,dealtV_WR);
    IOWR(0x10,0,tri_div_WR);
    IOWR(0x10,0,KW_WR);
    IOWR(0x10,0,pwm_cycle_WR);
}

void Data_Process()
{
/*************************计算交流功率因数******************************/
    V_Rms2 = V_Rms2 << 32;
    V_Rms1 = V_Rms1 << 16;
    I_Rms2 = I_Rms2 << 32;
    I_Rms1 = I_Rms1 << 16;
    I_Rms = sqrt(1.0*(I_Rms0 + I_Rms1 + I_Rms2)/Sample_num);
    V_Rms = sqrt(1.0*(V_Rms0 + V_Rms1 + V_Rms2)/Sample_num);
    P2 = P2 << 32;
    P1 = P1 << 16;
    Power = 1.0*(P0 + P1 + P2)/Sample_num;
    PF = Power/(I_Rms*V_Rms);
    if(V_Rms > 1900)
    {
        IOWR(0x10,0,0xC001);
        V_DC1_goal = (int) (0.6671 * I_Rms + 22587);
        adjust_pid_setPoint(&pid1,V_DC1_goal);
//        P5OUT |= BIT2;
        P5OUT |= BIT3;
        boost_state = 1;
    }
    else
    {
        V_DC1_goal = (int) (0.6671 * I_Rms + 18087);
        adjust_pid_setPoint(&pid1,V_DC1_goal);
//        P5OUT &= ~BIT2;
        P5OUT &= ~BIT3;
        boost_state = 0;
    }

/***************************转换**********************************/
    ch4_r = ch4 * 0.00125327 - 10.25839736;                     //ch4_r = ch4 * 0.00126880 - 10.37650438
    ch5_r = ch5 * 0.00125327 - 10.26090391;                     //ch5_r = ch5 * 0.00126555 - 10.37634269

/***************************动态矫正*******************************/

/***************************显示**********************************/
//    DispHex57At(0,1*4,key_value,2);
    DispDec57At(0,5*4,delay,3);
    DispDec57At(1,0,ch4,5);
    DispDec57At(2,0,ch5,5);
    DispFloat57At(1,48,V_Rms,5,3);
    DispFloat57At(2,48,I_Rms,5,3);
    //DispFloat57At(3,48,Power,8,3);
    DispDec57At(4,40,tri_div,3);
    DispDec57At(5,4,KW,3);
    DispFloat57At(0,16*4,PF,1,3);
    DispDec57At(6,19*4,V_DC1_goal,5);
}

void AD(void)
{
/*************************获取AD数据********************************/
    key_value = IORD(0,0);
    __delay_cycles(100000);
    ch4 = IORD(0x10,0);                     //voltage
    ch5 = IORD(0x20,0);                     //current
    //Max_V = IORD(0x30,0);
    //Max_I = IORD(0x50,0);
    V_Rms2 = IORD(0x40,0);
    V_Rms1 = IORD(0x40,0x01);             //second argument -> dataAddr[7:0]
    V_Rms0 = IORD(0x40,0x02);
    I_Rms2 = IORD(0x40,0x03);
    I_Rms1 = IORD(0x40,0x04);
    I_Rms0 = IORD(0x40,0x05);
    Sample_num = IORD(0x40,0x06);
    P2 = IORD(0x40,0x07);
    P1 = IORD(0x40,0x08);
    P0 = IORD(0x40,0x09);
}
void Key_CTR()
{
    switch(key_value)
    {
    case 0x18:
        delay = (delay<95)? delay + 1 : delay;              //移相+
        IOWR(0x10,0,delay);
//          IOWR(0x10,0,10);// PLAN B
//          IOWR(0x10,0,0);
        break;
    case 0x28:
        delay = (delay>0)? delay - 1 : delay;               //移相-
        IOWR(0x10,0,delay);
//        delay = 10 | 0xC000;// PLAN B
//        IOWR(0x10,0,delay);
//        IOWR(0x10,0,0xC000);
        break;
    case 0x48:
//        dealtV += 0.001;                                      //模拟PID, 调节I_set+
//        dealtV_WR = ((int)(dealtV * 2048)) | 0x2000;
//        IOWR(0x10,0,dealtV_WR);
        break;
    case 0x88:
//        dealtV = (dealtV>0)? dealtV - 0.001 : dealtV;         //模拟PID, 调节I_set-
//        dealtV_WR = ((int)(dealtV * 2048)) | 0x2000;
//        IOWR(0x10,0,dealtV_WR);
        break;
    case 0x81:
        tri_div++;                                          //调节三角波幅值+
        tri_div_WR = tri_div | 0x4000;
        IOWR(0x10,0,tri_div_WR);
        break;
    case 0x84:
        tri_div = (tri_div>1)? tri_div - 1 : tri_div;       //调节三角波幅值-
        tri_div_WR = tri_div | 0x4000;
        IOWR(0x10,0,tri_div_WR);
        break;
    case 0x11:
        KW++;                                               //调节三角波频率+
        KW_WR = KW | 0x6000;
        IOWR(0x10,0,KW_WR);
        break;
    case 0x12:
        KW = (KW>1)? KW - 1 : KW;                           //调节三角波频率-
        KW_WR = KW | 0x6000;
        IOWR(0x10,0,KW_WR);
        break;
    case 0x14:
//        P += 0.01;
//        DispFloat57At(6,1*4,P,1,3);
        pwm_cycle += 5;                                        //调节占空比+
        pwm_cycle_WR = pwm_cycle | 0x8000;
        IOWR(0x10,0,pwm_cycle_WR);
        DispDec57At(7,4,pwm_cycle,4);
        break;
    case 0x24:
//        P = (P>0)? P - 0.01 : P;
//        DispFloat57At(6,1*4,P,1,3);
        pwm_cycle = (pwm_cycle>5)? pwm_cycle - 5 : pwm_cycle; //调节占空比-
        pwm_cycle_WR = pwm_cycle | 0x8000;
        IOWR(0x10,0,pwm_cycle_WR);
        DispDec57At(7,4,pwm_cycle,4);
        break;
    case 0x22:                                                  //开启PWM
//        I += 0.01;
//        DispFloat57At(6,10*4,I,1,3);
        pwm_en = (pwm_en == 0)? 1 : 0;
        if(pwm_en)
            IOWR(0x10,0,0xC001);
        else
            IOWR(0x10,0,0xC000);
        break;
    case 0x42:
//        I = (I>0)? I - 0.01 : I;
//        DispFloat57At(6,10*4,I,1,3);

        break;
    case 0x41:
        V_DC1_goal += 5;
        DispDec57At(6,19*4,V_DC1_goal,5);
        adjust_pid_setPoint(&pid1,V_DC1_goal);
        break;
    case 0x44:
        V_DC1_goal = (V_DC1_goal>5)? V_DC1_goal - 5 : V_DC1_goal;
        DispDec57At(6,19*4,V_DC1_goal,5);
        adjust_pid_setPoint(&pid1,V_DC1_goal);
        break;
    case 0x21:                                                  //开启SPWM
        spwm_en = (spwm_en == 0)? 1 : 0;
        if(spwm_en)
            IOWR(0x10,0,0xA001);
        else
            IOWR(0x10,0,0xA000);
        break;
    case 0x82:
        state = (state == 0)? 1 : 0;
//        if(state == 0)
//        {
//            dealtV = 1.001;
//            dealtV_WR = ((int)(1.001 * 2048)) | 0x2000;
//            IOWR(0x10,0,dealtV_WR);
//        }
        break;
    default:break;
    }
//    adjust_pid(&pid1,P,I,D);
}

void pid1Adjust()
{
    dealtV += PidDeltaCal(&pid1,V_DC1)*0.000002;
    show(dealtV,4,1,1,3);
    dealtV_WR = ((int)(dealtV * 2048)) | 0x2000;
    IOWR(0x10,0,dealtV_WR);
}

void pid2Adjust()
{
    if(I_Rms < 75)
        pwm_cycle = 2005;
    else if(I_Rms < 194)
        pwm_cycle = 2010;
    DispDec57At(7,4,pwm_cycle,4);
    pwm_cycle_WR = pwm_cycle | 0x8000;
    IOWR(0x10,0,pwm_cycle_WR);
}

void main( void )
{
    WDTCTL = WDTPW + WDTHOLD;

    BUS_Init();
    Lcd_Init();
    __delay_cycles(1000000);
    FPGA_Init();
    Timer_Init();
    ADS1118_init();
    ADS1118_init2();
    GPIO_Init();
    PidDeltaInit(&pid1, V_DC1_goal, -1000, 1000, P, I, D);
//    DispFloat57At(6,1*4,P,1,3);
//    DispFloat57At(6,10*4,I,1,3);
    DispDec57At(6,19*4,V_DC1_goal,5);
    DispString57At(0,11*4,"PF:");
    _EINT();
    while(1);
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
{
    AD();
    Key_CTR();
    Data_Process();
    DispDec57At(0,0*4,state,2);                          //show the current state
//    switch(state)
//    {
//    case 0:
//        break;
//    case 1:
//        break;
//    case 2:
//        break;
//    case 3:
//        break;
//    case 4:
//        break;
//    default:break;
//    }
    TA0CCTL0 &= ~CCIFG;
}

#pragma vector=TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
{
    V_DC1 = ADS1118_Get_U();                //get the DC voltage1
    DispDec57At(3,4,V_DC1,5);
    if(state == 1)
        pid1Adjust();
    V_DC2 = ADS1118_Get_U2();
    DispDec57At(3,48,V_DC2,5);
    if(boost_state == 1)
    {
        pid2Adjust();
    }
    TA1CCTL1 &= ~CCIFG;
}
