//clcd.c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

//#define _MAIN
extern void initDevices(void);
extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void writeData(char c);
extern void gotoLcdXY(unsigned char x, unsigned char y);
extern void Mes();
extern void stop1();
extern void test();

extern unsigned char on;
extern unsigned char Message;
extern unsigned char reci;
extern unsigned char Speaker;
extern unsigned char music;
extern unsigned char stop;
extern int c;
extern char m[32];
extern char m1[32];


int i;
extern char t[];

void flushClock(void);

#ifdef _MAIN
int main(void)
{
   initDevices();
   initLcd();
   while(1) flushClock();
}
#endif
static unsigned char  	flush=0;
void
updateClock(void)
{
	flush=1;
	if(on) { 
		flushClock();
		if(Message) Mes();
		if(stop) stop1();
		if(!stop) printLcd(2, 1, " ");
	}
	gotoLcdXY(1, 2);
	if(!on) test();
}

void
stop1(void)
{
    printLcd(1, 1, t);
	printLcd(2, 1, t);
	printLcd(1, 1, "Stop");
}
void
test(void)
{
    printLcd(1, 1, t);
	printLcd(2, 1, t);
}


void
Mes(void)
{
	int temp = 0;
	initLcd();
	for(int index_i = 1; index_i < 3; index_i++)
  	{
		for(int index_j = 1; index_j <= 16; index_j++)
		{
			gotoLcdXY(index_j, index_i);
			writeData(m[temp++]);

		}
		
	}	
   /* gotoLcdXY(1, 1);
   writeData(m[0]);
   gotoLcdXY(2, 1);
   writeData(m[1]);
   gotoLcdXY(3, 1);
   writeData(m[2]);
   gotoLcdXY(4, 1);
   writeData(m[3]);
   gotoLcdXY(5, 1);
   writeData(m[4]);
   gotoLcdXY(6, 1);
   writeData(m[5]);
   gotoLcdXY(7, 1);
   writeData(m[6]);
   gotoLcdXY(8, 1);
   writeData(m[7]);
   gotoLcdXY(9, 1);
   writeData(m[8]);
   gotoLcdXY(10, 1);
   writeData(m[9]);
   gotoLcdXY(11, 1);
   writeData(m[10]);
   gotoLcdXY(12, 1);
   writeData(m[11]);
   gotoLcdXY(13, 1);
   writeData(m[12]);
   gotoLcdXY(14, 1);
   writeData(m[13]);
   gotoLcdXY(15, 1);
   writeData(m[14]);
   gotoLcdXY(16, 1);
   writeData(m[15]);
   gotoLcdXY(1, 2);
   writeData(m[16]);
   gotoLcdXY(2, 2);
   writeData(m[17]);
   gotoLcdXY(3, 2);
   writeData(m[18]);
   gotoLcdXY(4, 2);
   writeData(m[19]);
   gotoLcdXY(5, 2);
   writeData(m[20]);
   gotoLcdXY(6, 2);
   writeData(m[21]);
   gotoLcdXY(7, 2);
   writeData(m[22]);
   gotoLcdXY(8, 2);
   writeData(m[23]);
   gotoLcdXY(9, 2);
   writeData(m[24]);
   gotoLcdXY(10, 2);
   writeData(m[25]);
   gotoLcdXY(11, 2);
   writeData(m[26]);
   gotoLcdXY(12, 2);
   writeData(m[27]);
   gotoLcdXY(13, 2);
   writeData(m[28]);
   gotoLcdXY(14, 2);
   writeData(m[29]);
   gotoLcdXY(15, 2);
   writeData(m[30]);
   gotoLcdXY(16, 2);
   writeData(m[31]); */
}


void
flushClock(void)
{
    if(!flush) return;
	flush = 0;
	if(on && (!Message)){
		printLcd(1, 1, t);
	  	printLcd(1, 1, "go run together!");
	 	printLcd(2, 1, t);
		if(Speaker) {
			switch(music) {
	        	case 1: printLcd(1, 1, t); printLcd(2, 1, t); printLcd(1, 1, "Sound of Music"); break;
				case 2: printLcd(1, 1, t); printLcd(2, 1, t); printLcd(1, 1, "City of Star"); break;
				case 3: printLcd(1, 1, t); printLcd(2, 1, t); printLcd(1, 4, "Power up!"); break; 
				case 4: printLcd(1, 1, t); printLcd(2, 1, t); printLcd(1, 2, "London Bridge"); break;
			}		 
		}
	if(reci) printLcd(2, 16, "M");
	if(Message) printLcd(2, 16, " ");	  
	}
}
