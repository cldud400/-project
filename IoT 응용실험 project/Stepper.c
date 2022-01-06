//stepper.c
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define _MAIN

extern void initDevices(void);

extern void initLcd(void);
extern void printLcd(int row, int col, char *str);
extern void setMotorSpeed(short ds);
extern void flushFndDigit(void);
extern void flushClock(void);
extern void playPiano(void);
extern void playMusic(void);
extern void playMusic1(void);
extern void playMusic2(void);
extern void playMusic3(void);
extern void playMusic4(void);

extern unsigned char Speaker;
extern unsigned char music;

extern unsigned char	on;
extern short			ds;



#ifdef _MAIN
int main(void)
{
 
   initDevices();
   initLcd();
   setMotorSpeed(ds); 
   
   while(1){
      flushFndDigit();

  }
}
#endif
