#include <msp430.h>
#include <math.h>
#include "CTS_Layer.h"



unsigned char values[12][4] =
{
        {0x18, 0x18, 0x18, 0x18}, // plus
        {0x38, 0xFE, 0xFE, 0x38}, // minus
        {0xFE, 0xC6, 0xC6, 0xFE}, // num0
        {0x66, 0xFE, 0xFE, 0x06}, // num1
        {0x46, 0xCE, 0xDA, 0x72}, // num2
        {0x44, 0x92, 0x92, 0x6c}, // num3
        {0xE0, 0x30, 0x30, 0xFE}, // num4
        {0xE2, 0xA2, 0xA2, 0x9c}, // num5
        {0x7C, 0xA2, 0xA2, 0x9C}, // num6
        {0x86, 0x9c, 0xB0, 0xE0}, // num7
        {0x6C, 0x92, 0x92, 0x6c}, // num8
        {0xF2, 0x92, 0x92, 0xFe}  // num9
};


uint16_t values_sensor;
uint16_t TRASHOLD_SENSOR = 65535;

long int cur_number = 0;
char col_addr = 30;
//char col_addr = 0;
int COUNT_CLEAR_PAGE = 8;

void wait_for(long int value)
{
    volatile long int i;
    for(i = 0; i < value; i++);
}

void write_data(unsigned char data)
{
    while (!(UCB1IFG & UCTXIFG));

    P7OUT &= ~BIT4;
    UCB1TXBUF = data;

    while (UCB1STAT & UCBUSY);
    P7OUT |= BIT4;
}

void init(void)
{
    P5DIR |= BIT6;
    P5DIR |= BIT7;
    P7DIR |= BIT4;
    P7DIR |= BIT6;
    P4SEL |= BIT1;
    P4SEL |= BIT3;
    P4DIR |= BIT1;
    P4DIR |= BIT3;
    P7OUT |= BIT4;
    P7OUT |= BIT6;

    P5OUT &= ~BIT7;
    P5OUT |= BIT7;

    UCB1CTL1 |= UCSWRST | UCSSEL_2;
    UCB1CTL0 |= UCSYNC | UCMSB | UCMST | UCCKPH;
    UCB1BR0 = 0x01;
    UCB1BR1 = 0;
    UCB1CTL1 &= ~UCSWRST;

    P5OUT &= ~BIT6;
    write_data(0x40);
    //write_data(0xA1);
    write_data(0xC0);
    write_data(0xA4);
    write_data(0xA6);
    write_data(0x2F);
    write_data(0xAF);
}

void set_addr(char col, char page) {

    P5OUT &= ~BIT6;

    char low_part = col & 0x0F;
    char high_part = col >> 4;

    write_data(low_part);
    write_data(0x10 | high_part);

    page &= 0x0F;
    write_data(0xB0 | page);
}

void clear_page(int page)
{
    int j;
    for (j = 0; j < 132; j++)
    {
        P5OUT &= ~BIT6;
        set_addr(j, page);
        P5OUT |= BIT6;
        write_data(0);
    }
}

void clear()
{
    int i;
    for(i = 0; i < COUNT_CLEAR_PAGE; i++)
    {
        clear_page(i);
    }

}

int get_len_number(long int number)
{
    if(number >= 1000000000)
    {
        return 10;
    }
    if(number >= 100000000)
    {
        return 9;
    }
    if(number >= 10000000)
    {
        return 8;
    }
    if(number >= 1000000)
    {
        return 7;
    }
    if(number >= 100000)
    {
        return 6;
    }
    if(number >= 10000)
    {
        return 5;
    }
    if(number >= 1000)
    {
        return 4;
    }
    if(number >= 100)
    {
        return 3;
    }
    if(number >= 10)
    {
        return 2;
    }
    return 1;
}

long long int _pow(long long int value, int degree)
{
    if(degree < 0)
    {
        return 1;
    }
    int i;
    int temp = value;
    for(i = 0; i < degree; i++)
    {
        value*=temp;
    }
    return value;
}


void print_numbers(void)
{
    long int number = cur_number;
    number = number < 0 ? -1*number : number;
    long long int div_value = _pow(10,get_len_number(number) - 2);


    const unsigned char *value_signed = cur_number < 0 ? values[0] : values[1];

    int j;
    for(j = 0; j < 4; j++)
    {
        P5OUT &= ~BIT6;
        set_addr(col_addr + j,7);
        P5OUT |= BIT6;
        write_data(value_signed[j]);
    }

    int i = 1;
    while(div_value)
    {
        long int val = number/div_value;
        number -= val*div_value;
        div_value /= 10;

        for(j = 0; j < 4; j++)
        {
            P5OUT &= ~BIT6;
            set_addr(col_addr + (i*5) + j,7);
            P5OUT |= BIT6;
            write_data(values[val + 2][j]);
        }
        i++;
    }
}

void setupAcp()
{
    ADC12CTL1 &= ~(ADC12CSTARTADD0 | ADC12CSTARTADD1 | ADC12CSTARTADD2 | ADC12CSTARTADD3);
    ADC12CTL1 &= ~(ADC12SHS0 | ADC12SHS1);
    ADC12CTL1 |= ADC12SSEL0 | ADC12SSEL1;

    ADC12CTL1 &= ~ADC12SHP;

    ADC12CTL0 |= ADC12MSC;
    ADC12CTL2 |= ADC12RES_3;
    ADC12CTL0 |= ADC12REFON;

    ADC12CTL1 |= ADC12CONSEQ_0;
    ADC12CTL1 |= ADC12DIV_2;
    ADC12CTL2 |= ADC12PDIV;

    ADC12CTL0 &=~ ADC12REF2_5V;

    ADC12MCTL0 |= ADC12SREF_1;
    ADC12CTL2 |= ADC12SR;
    ADC12CTL2 |= ADC12TCOFF;

    REFCTL0 &=~ REFMSTR;
    ADC12MCTL0 |= ADC12INCH_5;
    ADC12MCTL0 |= ADC12EOS;
}

void pot_init()
{
	P6SEL |= BIT5;
	P6DIR &= ~BIT5;
	P8SEL &= ~BIT0;
	P8DIR |= BIT0;
	P8OUT |= BIT0;
}


void startAcp()
{
	ADC12CTL0 |= ADC12ON;
    ADC12CTL0 |= ADC12ENC;
    ADC12IE |= ADC12IE0;
    ADC12IFG &= ~ADC12IFG0;
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC_int(void)
{
	P1OUT |= BIT2;
	clear();
	int temp = ADC12MEM0;
	cur_number = temp * 1.5 / 4096 * 1000;
    print_numbers();
	ADC12IFG &= ~ADC12IFG0;
}



void setVcoreUp(uint16_t level)
{
	// Open PMM registers for write
	PMMCTL0_H = PMMPW_H;
	// Set SVS/SVM high side new level
	SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
	// Set SVM low side to new level
	SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
	// Wait till SVM is settled
	while ((PMMIFG & SVSMLDLYIFG) == 0);
	// Clear already set flags
	PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
	// Set VCore to new level
	PMMCTL0_L = PMMCOREV0 * level;
	// Wait till new level reached
	if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
	// Set SVS/SVM low side to new level
	SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
	// Lock PMM registers for write access
	PMMCTL0_H = 0x00;
}

void main(void)
{
 	WDTCTL = WDTPW + WDTHOLD;

    P1DIR = 0xFF;
    P2DIR = 0xFF;
    P8DIR = 0xFF;
    P1OUT = 0;
    P2OUT = 0;
    P8OUT = 0;

    init();
    clear();
    pot_init();
    setupAcp();
    startAcp();

    setVcoreUp(0x01);
    setVcoreUp(0x02);
    setVcoreUp(0x03);

    UCSCTL3 = SELREF_2;
    UCSCTL4 |= SELA_2;

    __bis_SR_register(SCG0);
    UCSCTL0 = 0x0000;
    UCSCTL1 = DCORSEL_7;
    UCSCTL2 = FLLD_1 + 762;
    __bic_SR_register(SCG0);

    wait_for(1000000);

    TI_CAPT_Init_Baseline(&slider);
    TI_CAPT_Update_Baseline(&slider,1);

    _enable_interrupts();
    while (1)
    {
    	values_sensor = TI_CAPT_Slider(&slider);
        if(values_sensor < TRASHOLD_SENSOR)
        {
        	ADC12CTL0 |= ADC12SC;
			wait_for(100);
        	ADC12CTL0 &= ~ADC12SC;
        }
        else
        {
        	P1OUT &= ~BIT2;
        	clear_page(7);
        }
    }
}


