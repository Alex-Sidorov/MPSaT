#include <msp430.h> 

const int WAIT_TIME = 500000;

void wait(int time)
{
    volatile int i;
    for(i = 0; i < time; i++);
}

/*int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// MUST!!!!

    P2DIR |= 0xff;
    P1DIR |= 0xff;

    P1OUT |= BIT7;
    P2OUT |= BIT2;

	P2DIR &= 0;
	P1DIR &= 0;

	P1IN &= 0;
	P2IN &= 0;
	P8IN &= 0;

    P8DIR |= 0xff;
	P8OUT &= 0;

	P1REN |= BIT7;
    P2REN |= BIT2;

    P2SEL &= 0;
    P8SEL &= 0;
    P1SEL &= 0;

	while(1)
	{
	    if(!(P8OUT & BIT2))
	    {
            while((P1IN & BIT7));
            if(!(P2IN & BIT2))
            {
                P8OUT |= BIT2;
            }
	    }
	    else if(!(P1IN & BIT7))
	    {
            while(!(P1IN & BIT7));
            if(!(P2IN & BIT2))
            {
                P8OUT &= ~BIT2;
            }
	    }
	}
	return 0;
}*/

#pragma vector = PORT1_VECTOR
__interrupt void ISR1(void)
{
    if(!(P2IN & BIT2))
    {
        P8OUT ^= BIT2;
        P1IES ^= BIT7;
    }
    P1IFG &= ~BIT7;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;    // MUST!!!!

    P2DIR |= 0xff;
    P1DIR |= 0xff;

    P1OUT |= BIT7;
    P2OUT |= BIT2;

    P2DIR &= 0;
    P1DIR &= 0;

    P1IN &= 0;
    P2IN &= 0;
    P8IN &= 0;

    P8DIR |= 0xff;
    P8OUT &= 0;

    P1REN |= BIT7;
    P2REN |= BIT2;

    P2SEL &= 0;
    P8SEL &= 0;
    P1SEL &= 0;

    __bis_SR_register(GIE);

    P1IES |= BIT7;
    P1IE |= BIT7;

    __no_operation();
    return 0;
}
