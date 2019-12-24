#include <msp430.h> 


int is_dcoclk = 0;
int is_lpm = 0;

void wait()
{
    volatile int i;
    for(i = 0; i < 1000; i++);
}

#pragma vector = PORT1_VECTOR
__interrupt void change_lpm(void)
{
    if(is_lpm == 0)
    {
		__bis_SR_register_on_exit(LPM4_bits);
        is_lpm = 1;
    }
    else
    {
        __bic_SR_register_on_exit(LPM4_bits);
        is_lpm = 0;
    }
    //wait();

    P1IFG &= ~BIT7;
}

#pragma vector = PORT2_VECTOR
__interrupt void change_frequency(void)
{
    if(is_dcoclk == 0)
    {
        UCSCTL4 = SELM__DCOCLK;
		UCSCTL5 = DIVM__32;
        is_dcoclk = 1;
    }
    else
    {
		UCSCTL4 = SELM__XT1CLK;
		UCSCTL5 = DIVM__16;
        is_dcoclk = 0;
    }
    //wait();

    P2IFG &= ~BIT2;
}


int main(void)
{

	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    P7DIR |= BIT7;
    P7SEL |= BIT7;

    P1SEL &= ~BIT7;
    P1DIR &= ~BIT7;
    P1REN |= BIT7;
    P1OUT |= BIT7;
    P1IES |= BIT7;
    P1IFG &= ~BIT7;

	P2SEL &= ~BIT2;
    P2DIR &= ~BIT2;
    P2REN |= BIT2;
    P2OUT |= BIT2;
    P2IES |= BIT2;
    P2IFG &= ~BIT2;

	UCSCTL1 = DCORSEL_0;
    UCSCTL3 |= SELREF__XT1CLK;
    /*UCSCTL3 |= FLLREFDIV_1;
	UCSCTL2 |= FLLD_1;
	UCSCTL2 |= FLLN2;
	UCSCTL2 &= ~FLLN4;
	UCSCTL2 &= ~FLLN5;
	UCSCTL2 |=  FLLN3;
	UCSCTL2 &= ~FLLN1;
	UCSCTL2 &= ~FLLN0;*/

    /*UCSCTL3 |= FLLREFDIV_1;
    UCSCTL2 |= FLLD_1;
    UCSCTL2 &= ~FLLN2;
    UCSCTL2 &= ~FLLN4;
    UCSCTL2 &= ~FLLN5;
    UCSCTL2 |= FLLN3;
    UCSCTL2 &= ~FLLN1;
    UCSCTL2 &= ~FLLN0;*/

    UCSCTL3 |= FLLREFDIV_1;
    UCSCTL2 |= FLLD_1;
    UCSCTL2 |= FLLN2;
    UCSCTL2 &= ~FLLN4;
    UCSCTL2 &= ~FLLN5;
    UCSCTL2 |=  FLLN3;
    UCSCTL2 &= ~FLLN1;
    UCSCTL2 &= ~FLLN0;
	//UCSCTL2 |= FLLN2;//399kHZ
	//UCSCTL2 |= FLLN3|FLLN2;//468kHZ

    UCSCTL4 = SELM__DCOCLK;
    //UCSCTL5 = DIVM__32;
    is_dcoclk = 1;


    __bis_SR_register(GIE);
    P2IE |= BIT2;
    P1IE |= BIT7;

    __no_operation();
	return 0;
}
