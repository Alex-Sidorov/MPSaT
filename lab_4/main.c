#include <msp430.h>

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

int cur_number = +9531;
int add_number = -891;

char col_addr = 30;
int is_invers = 0;

void wait_for(int delay)
{
    volatile int i = 0;
    for(i=0; i<delay; i++);
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

void clear(void)
{
    int i, j;
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 132; j++)
        {
            P5OUT &= ~BIT6;
            set_addr(j, i);
            P5OUT |= BIT6;
            write_data(0);
        }
    }
}

int get_len_number(int number)
{
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

int _pow(int value, int degree)
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
    int number = cur_number;
    number = number < 0 ? -1*number : number;
    int div_value = _pow(10,get_len_number(number) - 2);


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
        int val = number/div_value;
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

#pragma vector = PORT1_VECTOR
__interrupt void int_s1(void)
{
    wait_for(2000);

    if(!(P1IN & BIT7))
    {
        cur_number += add_number;
        clear();
        print_numbers();
    }

    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void int_s2(void)
{
    wait_for(10000);

    if(!(P2IN & BIT2))
    {
        P5OUT &= ~BIT6;
        char cmd = 0xA6;
        if(is_invers == 0)
        {
            is_invers = 1;
            write_data(cmd + 1);
        }
        else
        {
            is_invers = 0;
            write_data(cmd);
        }
        wait_for(10000);
    }
    P2IFG &= ~BIT2;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    P1DIR &= ~BIT7;
    P1OUT |= BIT7;
    P1REN |= BIT7;
    P1IES |= BIT7;
    P1IFG &= ~BIT7;

    P2DIR &= ~BIT2;
    P2OUT |= BIT2;
    P2REN |= BIT2;
    P2IES |= BIT2;
    P2IFG &= ~BIT2;

    init();
    clear();
    print_numbers();

    __bis_SR_register(GIE);

    P1IE |= BIT7;
    P2IE |= BIT2;

    __no_operation();
    return 0;
}
