#include	<avr/io.h>
#include	<avr/interrupt.h>
#include	<stdio.h>

#define		_LED
#define		_SPEAKER
#define		_FND
#define     _DOT_MATRIX
#define     _CLCD
#define     _UART
#define     _STEPPER

#define     EX_CLCD_CONTROL     (* (volatile unsigned char *) 0x8001)
#define     EX_CLCD_DATA        (* (volatile unsigned char *) 0x8000)
#define		EX_FND_DATA			(* (volatile unsigned char *) 0x8002)
#define		EX_FND_SELECT		(* (volatile unsigned char *) 0x8003)
#define     EX_DOT_MATRIX_LINE  (* (volatile unsigned char *) 0x8004)
#define     EX_DOT_MATRIX_DATA  (* (volatile unsigned char *) 0x8006)
#define		EX_LED				(* (volatile unsigned char *) 0x8008)
#define		EX_STEPPER			(* (volatile unsigned char *) 0x8009)


extern void updateLed(void);
extern void onM(void);
extern void ledUpdate(void);
extern void speedled(void);
extern void	updateFndDigits(void);
extern void turnDotMatrixPage(void);
extern void fndCountUp(int incr);
extern void updateClock(void);
extern void editCharLcd(void);
extern void playMusic(void);


unsigned char	on = 0;
short           ds = 0; //velocity contorl variable
unsigned char   Message = 0;
unsigned char   Speaker = 0;
static short	sm1Count = 0;
static short	sm1Step = 0;
short			sm1Speed = 1; //if 0 stop, 1 right, -1 left!
unsigned char   reci = 0;
unsigned char   music = 0;
unsigned char   stop = 0;
extern int		ledData;
char			t[]="                                  ";



void		printLcd(int row, int col, char *str);
void		setMotorSpeed(void);
static void	updateFnd();
static void toggleSpeaker(void);
static void updateDotMatrix(void);
static void stepMotor(void);

extern unsigned char musicNote;



static void initPorts(void)
{
	DDRG = 0x10;
}

static void initTimer0(void)
{
	TCCR0 = 0x00;
	TCNT0 = 0x06; ///250개의 카운터
	TCCR0 = 0x04; /// 64분의1            1kHz
}

static void initTimer1(void)
{
	TCCR1B = 0x00;
	TCNT1L = 0x00;
	TCNT1H = 0x00;
	TCCR1B = 0x01;	///타이머 0과 1을 쓰기위해서
}

static void initTimer2(void)
{
    TCCR2=0x00;
	OCR2 =0x13;
	TCNT2=0x00;
	TCCR2=0x0a;
}

static void initTimer3(void)
{
	TCCR3B = 0x00;
	TCNT3H = 0xc2;
	TCNT3L = 0xf7;
	TCCR3B = 0x05;
}



unsigned char	musicKey = 100;
unsigned int	musicScale[] = {
	0xc440, 0xcac4, 0xd095, 0xd33b, 0xd817, 0xdc7e, 0xe05c, 0xe223, 0xe565, 0xe84f, 0xec12, 0x0000, 0xc0be, 0xb8fa, 0xb048, 0xe9a2
	};

void
toggleSpeaker(void)
{
	if(musicKey < 15) {
		TCNT1H = musicScale[musicKey] >> 8;
		TCNT1L = musicScale[musicKey] & 0xff;
		PORTG ^= 0x10;
	} else
		PORTG &= 0xef;
}





ISR(TIMER0_OVF_vect)		//interrupt service routine
{
	TCNT0 = 0x06;
#ifdef	_LED
    if(reci && on) onM();
	else EX_LED=0x00;
#endif
#ifdef	_SPEAKER
    playMusic();
#endif
#ifdef	_FND
	updateFnd();
#endif
}

ISR(TIMER1_OVF_vect)
{
#ifdef	_SPEAKER
	toggleSpeaker();
#endif
}
ISR(TIMER2_COMP_vect) {
	TCNT2=0xce;
#ifdef _DOT_MATRIX
	updateDotMatrix();
#endif
#ifdef _STEPPER
	if(on && (ds > 0) && !stop) stepMotor();
#endif
}
ISR(TIMER3_OVF_vect)
{
	TCNT3H = 0xc2;
	TCNT3L = 0xf7;
#ifdef	_FND
	updateFndDigits();
#endif
#ifdef _DOT_MATRIX
    turnDotMatrixPage();
#endif
#ifdef _CLCD
    updateClock();
#endif
#ifdef _STEPPER
    setMotorSpeed();
#endif
}
unsigned char				fndDigit[4] = { 0, 0, 0, 0 };
static unsigned char		fndPosition = 0;
static const unsigned char	fndSegment[] = {
	0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67
};
static const unsigned char  fndSegmentd[] ={
    0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfc, 0x87, 0xff, 0xd7
};
//10 a 11 b 12 c 13 d 14 e 15f

static void
updateFnd(void)
{
	EX_FND_SELECT = 0x0f;
	if(fndPosition != 1) EX_FND_DATA = fndSegment[fndDigit[fndPosition]];
	if(fndPosition == 1) EX_FND_DATA = fndSegmentd[fndDigit[fndPosition]];
	if(on == 0) EX_FND_DATA = 0x00;
	EX_FND_SELECT = ~(0x01 << fndPosition);				////선택된 부분을 전부 0으로 만들어줌

	fndPosition++;
	fndPosition %= 4;
}
short                      *dotMatrixData = (short*)NULL;

static unsigned char       dotMatrixLine = 0;
static unsigned char       dmCount = 0;
static void
updateDotMatrix(void)
{
	if(!dotMatrixData) return;
	if(++dmCount <20) return;
	dmCount = 0;
	EX_DOT_MATRIX_LINE = 1 << dotMatrixLine;
	EX_DOT_MATRIX_DATA = dotMatrixData[dotMatrixLine];
	dotMatrixLine++;
	dotMatrixLine %= 10;
}
static char clcdControl=0;
#define LCD_EN_ON		(EX_CLCD_CONTROL=(clcdControl |= 0x04))
#define LCD_EN_OFF		(EX_CLCD_CONTROL=(clcdControl &= ~0x04))

#define LCD_RW_ON		(EX_CLCD_CONTROL=(clcdControl |= 0x02))
#define LCD_RW_OFF		(EX_CLCD_CONTROL=(clcdControl &= ~0x02))

#define LCD_RS_ON		(EX_CLCD_CONTROL=(clcdControl |= 0x01))
#define LCD_RS_OFF		(EX_CLCD_CONTROL=(clcdControl &= ~0x01))

void
delay(int n)
{
	for(volatile int i = 0; i < n; i++)	{
		for(volatile int j = 0; j<600; j++);
	}
}

void
delay2(int n)
{
    for(volatile int i = 0; i < n; i++) {
	   for(volatile int j = 0; j < 6; j++);
    }
}
void
writeCommand(char value)
{
    LCD_EN_OFF;
	delay2(1);
	LCD_RS_OFF;
	delay2(1);
	LCD_RW_OFF;
	delay2(1);
	LCD_EN_ON;
	delay2(1);
	EX_CLCD_DATA=value;
	delay2(1);
	LCD_EN_OFF;
	delay2(1);

}

void
writeData(char value)
{
    LCD_EN_OFF;
	delay2(1);
	LCD_RS_ON;
	delay2(1);
	LCD_RW_OFF;
	delay2(1);
	LCD_EN_ON;
	delay2(1);
	EX_CLCD_DATA = value;
	delay2(1);
	LCD_EN_OFF;
	delay2(1);
}
void
initLcd(void)
{
    writeCommand(0x38);
	writeCommand(0x0e);
	writeCommand(0x06);
	writeCommand(0x01);
	delay2(100);
	writeCommand(0x02);
	delay2(100);
}
void
writeString(char *str)
{
    while(*str)
	    writeData(*str++);
}
void
gotoLcdXY(unsigned char x, unsigned char y)
{
    switch(y){
	     case 1:
		     writeCommand(0x80 + x -1);
			 break;
         case 2:
		     writeCommand(0xc0 + x -1);
			 break;
	 }
}
void
printLcd(int row, int col, char *str)
{
    gotoLcdXY(col, row);
	writeString(str);
	  

}
#define ATOMIC(x) { char _sreg = SREG; cli(); x; txHead = (txHead+1) % BUFFSIZE; SREG= _sreg;}
#define BUFFSIZE 128

unsigned int txHead = 0, txTail = 0, rxHead = 0, rxTail = 0;
unsigned char txBuff[BUFFSIZE], rxBuff[BUFFSIZE];
static void
initUart1(void)
{
    UCSR1B = 0x00;
	UCSR1C = 0x06;
	UBRR1L = 0x67;
	UBRR1H = 0x00;
	UCSR1B = 0xb8;
}

ISR(USART1_RX_vect)
{
   if((rxHead + 1) % BUFFSIZE != rxTail) {
      rxBuff[rxHead] = UDR1;
	  rxHead = (rxHead + 1) % BUFFSIZE;
   }
#ifdef _UART
   editCharLcd();
#endif
}
ISR(USART1_UDRE_vect)
{
   if(txHead != txTail){
      UDR1 = txBuff[txTail];
	  txTail = (txTail + 1) % BUFFSIZE;
   }
}

static void
putChar(char c)
{
   if(txHead == txTail)
   {
      if(UCSR1A & (1 << UDRE))
	  {
	     UDR1=c;
		 return;
	  }
   } 
   else if((txHead + 1) % BUFFSIZE == txTail)
   {

         while(!(UCSR1A & (1 << UDRE)));
         UDR1 = txBuff[txTail];
		 ATOMIC(txTail = (txTail + 1) % BUFFSIZE);
   }
   txBuff[txHead] = c;
   
   ATOMIC(txHead = (txHead + 1) % BUFFSIZE);//racing 현상제거
		 //UDR1 = c;//while 밖 blocking
   
}
int
peekChar(void)
{
   if(rxTail == rxHead) return -1;
   return rxBuff[rxTail];

}
int
getChar(void)
{
   int c;
   if(rxTail == rxHead){
      while(!(UCSR1A & (1 << RXC1)));
      return UDR1;}
   c = rxBuff[rxTail];
   ATOMIC(rxTail = (rxTail + 1) % BUFFSIZE);
   return c;

}
void
initDevices(void)
{
	cli();
	initPorts();
	initTimer0();
	initTimer1();
	initTimer2();
	initTimer3();
	initUart1();
	fdevopen(putChar, 0);
	MCUCR = 0x80;
	TIMSK = 0x85;
	ETIMSK = 0x04;
	sei();
}


static unsigned char smStepPhase[] = {
       0x01, 0x03, 0x02, 0x06, 0x04,0x0c, 0x08, 0x09
      };
static short       sm1MaxCount = 250;

static short       smMaxCount[] = {250, 125, 83, 43 , 25}; //250, 125, 83, 62, 50, 41, 35, 31, 28, 25


#define MAXSPEED  ((short) (sizeof (smMaxCount) / sizeof (short)))

void
setMotorSpeed(void)
{

	 sm1Speed = 1;
	 sm1MaxCount = smMaxCount[ds];
}






#define		NSTEPS (sizeof(smStepPhase) / sizeof(unsigned char))



void
stepMotor(void)
{
	if(sm1Speed && (++sm1Count == sm1MaxCount)) {
			EX_STEPPER = smStepPhase[sm1Step];
			if(sm1Speed > 0) sm1Step = (sm1Step + 1) % NSTEPS;
			else if(--sm1Step < 0) sm1Step = NSTEPS - 1;
			sm1Count = 0;
		}
}


