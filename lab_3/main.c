#include <msp430.h>

int count = 0;
int is_a1 = 1;

int WAIT_TIME = 1000;

void wait(long value)
{
    volatile int i;
    for (i = 0; i < value; i++);
}

#pragma vector = PORT1_VECTOR
__interrupt void s1_int(void)
{
    wait(WAIT_TIME);

    if (!(P1IN & BIT7))
    {
        count = 0;

        P1SEL &= ~BIT3;
        P1OUT &= ~BIT3;

        if (is_a1)
        {
            TA1CTL = TASSEL_1 | ID__1 | MC__UP | TACLR;
            TA1CCR0 = 32768*0.3;
            TA1CCTL0 |= CCIE;
        }
        else
        {
            WDTCTL = WDTPW + (!WDTHOLD) + WDTTMSEL + WDTCNTCL + WDTIS__8192 + WDTSSEL__ACLK;
            SFRIE1 |=  WDTIE;
        }
    }
    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void s2_int(void)
{
    wait(WAIT_TIME);

    if (!(P2IN & BIT2))
    {
        P8OUT ^= BIT1;
        if (is_a1)
        {
            TA1CTL  = MC__STOP;
            TA1CCTL0 &= ~CCIE;
            is_a1 = 0;
        }
        else
        {
            WDTCTL = WDTPW + WDTHOLD;
            SFRIE1 &= ~WDTIE;
            is_a1 = 1;
        }
    }

    P2IFG &= ~BIT2; // clear interrupt request flag
}

#pragma vector = WDT_VECTOR
__interrupt void wdt_int(void)
{
    switch(count)
    {
       case 3:
           P1OUT |= BIT3;  //led 6
           break;
       case 6:
           P1OUT |= BIT4;  //led 7
           break;
       case 8:
           P1OUT &= ~BIT3; //led 6
           break;
       case 9:
           P1OUT |= BIT5;  //led 8
           break;
       case 12:
           P1OUT &= ~BIT4; //led 7
           break;
       case 14:
           P1OUT &= ~BIT5; //led 8

           SFRIE1 &= ~WDTIE;
           WDTCTL = WDTPW + WDTHOLD;


           P1SEL |= BIT3;

           count = 0;
           return;
       default:
           break;
    }
    count++;
}

#pragma vector = TIMER1_A0_VECTOR
__interrupt void a1_int (void)
{
     switch(count)
    {
        case 3:
            P1OUT |= BIT3;  //led 6
            break;
        case 6:
            P1OUT |= BIT4;  //led 7
            break;
        case 8:
            P1OUT &= ~BIT3; //led 6
            break;
        case 9:
            P1OUT |= BIT5;  //led 8
            break;
        case 12:
            P1OUT &= ~BIT4; //led 7
            break;
        case 14:
            P1OUT &= ~BIT5; //led 8

            TA1CTL  = MC__STOP;
            TA1CCTL0 &= ~CCIE;

            P1SEL |= BIT3;
            count = 0;
            return;
        default:
            break;
    }
    count++;
}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    P1DIR |= BIT3;  //led 6
    P1OUT &= ~BIT3;
    P1SEL |= BIT3;

    P8DIR |= BIT1;  //led 2
    P8OUT &= ~BIT1;

    P1DIR |= BIT4;  // led 7
    P1OUT &=~ BIT4;

    P1DIR |= BIT5;  // led 8
    P1OUT &= ~BIT5;

    P1DIR &= ~BIT7; // button s1
    P1OUT |= BIT7;
    P1REN |= BIT7;
    P1IES |= BIT7;
    P1IFG &= ~BIT7;

    P2DIR &= ~BIT2; // button s2
    P2OUT |= BIT2;
    P2REN |= BIT2;
    P2IES |= BIT2;
    P2IFG &= ~BIT2;

    TA0CTL = TASSEL_1 | ID__1  | MC__UPDOWN | TACLR;
    TA0CCTL2 = OUTMOD_7;
    TA0CCR0 = 32768/2;
    TA0CCR2 = 32768/4;


    P1IE |= BIT7;
    P2IE |= BIT2;

    __bis_SR_register(GIE);

    __no_operation();

    return 0;
}
