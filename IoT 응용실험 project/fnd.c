
#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

//#define	_MAIN
void flushFndDigit(void);


extern void 			initDevices(void);
extern unsigned char    on;
extern short           ds;
extern unsigned char   stop;
#ifdef	_MAIN
int main(void)
{
    initDevices();

    while(1) flushFndDigit();
}
#endif

extern unsigned char	fndDigit[];
static unsigned int		fndValue = 0;
static unsigned char    flush=0;


void
updateFndDigits(void)
{
	if(on && (ds != 0)){
	   if(!stop) fndValue++;
	   flush = 1;}
    if(!on) {fndValue = 0; flush = 1;}
    

}
void
flushFndDigit(void)
{   if(!flush) return;
    flush =0;

    fndDigit[0] = fndValue % 3600 / 600; 
	fndDigit[1] = fndValue % 3600 % 600 / 60; 
	fndDigit[2] = fndValue % 60 / 10; 
	fndDigit[3] = fndValue % 10; 
	
}
void
fndCountUp(int incr)
{
	fndValue += incr;
}

