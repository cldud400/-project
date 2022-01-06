#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

 

//#define _MAIN

 

#define		EX_LED		(* (volatile unsigned char *) 0x8008)

 

extern void initDevices(void);
extern short ds;
extern unsigned char on;
extern unsigned char reci;
extern void delay(int n);

 

 

 

#ifdef	_MAIN
int main(void)
{
    initDevices();
    while(1);
}
#endif

 

static unsigned char n1kHzCycles = 0;
int ledData = 0x01;
unsigned char dir=1;
void
updateLed(void)
{

     if(!on) ledData=0x00;
	 EX_LED=ledData;
}

void
onM(void)
{
	 n1kHzCycles++;
	 EX_LED = ledData;
	 if(n1kHzCycles > 200 && n1kHzCycles < 400) ledData = 0xff;
	 else ledData = 0x00;
	 if(n1kHzCycles == 1000) n1kHzCycles = 0;
}

void
ledUpdate(void)
{
    n1kHzCycles++;
	EX_LED = ledData;
	if(n1kHzCycles == 500) {
		if(dir) {
			ledData <<= 1;
			if(ledData == 0x80)
			dir = -1;
		}
		if(dir == -1) {
			ledData >>= 1;
			if(ledData == 0x01)
				dir = 1;
		}
	n1kHzCycles=0;
	}
}

 

void speedled(void)

{
	char i;
	n1kHzCycles++;
	EX_LED = ledData;
	if(reci == 0) ledData = 0x00;
	else if(reci == 1){
		for(i = 0; i < 5; i++){
			if(i == 0) {
				ledData=0x80;
				ledData=0x40;
				ledData=0x20;
				ledData=0x10;
				ledData=0x08;
				ledData=0x04;
				ledData=0x02;
				ledData=0x01;
			}
			if(i == 2) {
				ledData=0x80;
				ledData=0x40;
				ledData=0x20;
				ledData=0x10;
				ledData=0x08;
				ledData=0x04;
				ledData=0x02;
				ledData=0x01;
			}
			if(i == 4) {
				ledData=0x80;
				ledData=0x40;
				ledData=0x20;
				ledData=0x10;
				ledData=0x08;
				ledData=0x04;
				ledData=0x02;
				ledData=0x01;
			}
			if(i == 1) {
				ledData = 0x00;
			}
			if(i == 3) {
				ledData = 0x00;
			}
		}
	}
}

 
