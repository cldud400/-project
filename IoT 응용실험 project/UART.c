#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

//#define _MAIN

extern void initDevices(void);
extern void initUart1(void);
extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void gotoLcdXY(unsigned char x, unsigned char y);
extern void writeData(char c);
extern int 	getChar(void);
extern int 	peekChar(void);

extern unsigned char Message;
extern unsigned char reci;
#ifdef _MAIN
int main(void)
{
 
   initDevices();
   initLcd();
   
   while(1);
}
#endif

int		i = 0;
char	m[32]="                                ";
int		c;

void
editCharLcd(void)
{

   
	while(peekChar() > -1) {
		c = getChar();
		if(c != 46) m[i] = c;
		else if(c == 46) {
			printf("\n");
			reci = 1;
			i = i - 1;
		}
		if(c == 8) {		// BS
			printf("<-delete!");
			for(int k = 0; k < 32; k++) m[k] = 32;
		}
		if(!Message) {
			if(c != 8) printf("%d \r",c);
		}
 		i = (i + 1) % 32;
		if(c == 8) i = 0;	   
	}
}

