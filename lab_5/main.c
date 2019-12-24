
#include <msp430.h>
#include <math.h>

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

long int cur_number = 0;
char col_addr = 0;
int COUNT_CLEAR_PAGE = 8;

void wait_for(int value)
{
    volatile int i;
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
    write_data(0xA1);
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

void clear_all_page()
{
    int i,j;
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

char set_cma_reg(char first_byte, char second_byte)
{
    char data;

    P3OUT &= ~BIT5;  

    data = UCA0RXBUF;
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = first_byte;
    while(!(UCA0IFG & UCRXIFG));

    data = UCA0RXBUF;
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = second_byte;
    while(!(UCA0IFG & UCRXIFG));

    data = UCA0RXBUF;

    while(UCA0STAT & UCBUSY);

    P3OUT |= BIT5;

    return data;
}


void set_acc()
{
    P2DIR  &= ~BIT5;
    P2REN  |=  BIT5;
    P2IES  &= ~BIT5;
    P2IFG  &= ~BIT5;
    P2IE   |=  BIT5;


    P3DIR  |=  BIT5;
    P3OUT  |=  BIT5;

    P2DIR  |=  BIT7;
    P2SEL  |=  BIT7;


    P3DIR  |= (BIT3 | BIT6);
    P3DIR  &= ~BIT4;
    P3SEL  |= (BIT3 | BIT4);
    P3OUT  |= BIT6;

    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 = UCCKPH  & ~UCCKPL | UCMSB |  UCMST   |  UCSYNC  | UCMODE_0;
    UCA0CTL1 = UCSSEL_1 | UCSWRST;
    UCA0BR0 = 0x01;
    UCA0BR1 = 0x0;
    UCA0CTL1 &= ~UCSWRST;

    set_cma_reg(0x04, 0);
    wait_for(1250);
    set_cma_reg(0x0A, BIT7 | BIT4 | BIT1);
    wait_for(22000);

}

long int convert_mg(unsigned char projection_byte)
{
    unsigned char bits[] = { BIT6, BIT5, BIT4, BIT3, BIT2, BIT1, BIT0 };
    long int map[] = { 1142, 571, 286, 143, 71, 36, 18 };

    long int projection = 0;
    int i = 0;
    int count_bits = 7;
    for (; i < count_bits; i++)
    {
        if (!(projection_byte & BIT7))
        {
            projection += (bits[i] & projection_byte) ? map[i] : 0;
        }
        else
        {
            projection -= (bits[i] & projection_byte) ? 0 : map[i];
        }
    }
    return projection;
}


#pragma vector = PORT2_VECTOR
__interrupt void acces_int(void)
{
    clear_page(7);

    long int x_data = convert_mg(set_cma_reg(0x18, 0));
    long int y_data = convert_mg(set_cma_reg(0x1C, 0));
    long int z_data = convert_mg(set_cma_reg(0x20, 0));

    // 3600 * 3600 / (1000*1000) * 9.81 * 100
    long int coeff = 12713;

    cur_number = y_data * coeff;

    print_numbers();

    double angle = atan(y_data/sqrt(pow(x_data, 2) + pow(z_data, 2))) * 57.3;

    if ((angle >= -150) && (angle <= -30))
    {
        P8OUT |= BIT1;
    }
    else
    {
        P8OUT &= ~BIT1;
    }
    wait_for(25000);
    wait_for(25000);
}



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer


    P8DIR |= BIT1;
    P8OUT &= ~BIT1;

    init();
    clear_all_page();
    set_acc();

    __bis_SR_register(GIE);
    __no_operation();
    return 0;
}
