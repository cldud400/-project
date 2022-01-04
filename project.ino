
#include <DS1302.h>          // DS1302 RTC 시계모듈 라이브러리
#include <Wire.h>           // wire 라이브러리 (lcd통신)
#include <LiquidCrystal_I2C.h>  // LCD 라이브러리
#include <TimerOne.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define Red Color(255,0,0)
#define Green Color(0,255,0)
#define Blue Color(0,0,255)

#define PIN 13
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(4,PIN,NEO_GRB+NEO_KHZ800);


SoftwareSerial BTSerial(0,1);
byte buffer[1024];
int bufferPosition;

//0x3f I2C주소를 가지고 있는 16x2 lcd객체 생성 (i2c 주소는 lcd에 맞게 수정)
LiquidCrystal_I2C lcd(0x27,16,2);

//////////핀번호///////////
DS1302 rtc(2,3,4); //rtc 객체(연결된 핀정보 RST, IO, SCL)
int photo_1 = A0;
int photo_2 = A1;
int trig = A2;
int echo = A3;
//모터
int EN_1 = 8;
int EN_2 = 7;
int EN_3 = 6;
int EN_4 = 5;
int SW1 = 9;
int SW2 = 10;
int SW3 = 11;
int SW4 = 12;

///////////////////////

int Hour,Min,Sec;
int ModeCount=0;
int M_SW, U_SW, D_SW, C_SW;

//1의자리 10의자리
int alarm_h1,alarm_h2,alarm_m1,alarm_m2,alarm_s1,alarm_s2;


int alarm_h_set;
int alarm_m_set;
int alarm_s_set;

//시간 추출 변수
Time t;
int Hour_t,Min_t,Sec_t;

//상태변수들
int state;         //동작 상태 변수 0:평시,1:작동
int state_motor_1; //모터 1 상태
int state_motor_2; //모터 2 상태
int state_photo_1; //포토 인터럽터 1 상태
int state_photo_2; //포토 인터럽터 2 상태

char BT;
long duration,distance;

volatile bool LCD_Check = 0;

unsigned long preMillisOfDisplay = 0;

void setup()
{
  Serial.begin(9600); //시리얼 통신 시작
  BTSerial.begin(9600);
  bufferPosition = 0;
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(LCD);

  pinMode(photo_1,INPUT);
  pinMode(photo_2,INPUT);

  pinMode(M_SW,INPUT);
  pinMode(U_SW,INPUT);
  pinMode(D_SW,INPUT);
  pinMode(C_SW,INPUT);
  
  pinMode(EN_1, OUTPUT);       // Motor A 방향설정1
  pinMode(EN_2, OUTPUT);       // Motor A 방향설정2
  pinMode(EN_3, OUTPUT);       // Motor B 방향설정1
  pinMode(EN_4, OUTPUT);       // Motor B 방향설정2

  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  pixels.begin();
  pixels.show();
  
  rtc.halt(false);
  rtc.writeProtect(false);

  lcd.begin();         //i2c lcd 초기화
  lcd.backlight();    //백라이트 on

  state=0;
  state_motor_1=0;
  state_motor_2=0;
  state_photo_1=0;
  state_photo_2=0;

  alarm_h_set=1;
  alarm_m_set=1;
  alarm_s_set=0;

  alarm_h1=0;
  alarm_h2=0;
  alarm_m1=0;
  alarm_m2=0;
  alarm_s1=0;
  alarm_s2=0;
  Sec=0;

  BT='0';

  //*시간설정* 한번만업로드하고 주석처리후 재업로드
  rtc.setDOW(SUNDAY); //요일설정
  rtc.setTime(01,00,55); //시간설정 시,분,초
  rtc.setDate(07,06,2020); //날짜설정 일 월 년
  
}

void loop()
{
  if(state == 1)
  {
    pixels.setPixelColor(0,0,10,0);
    pixels.setPixelColor(1,0,10,0);
    pixels.show();
    state_motor_1 = 1;
    state_motor_2 = 1;
    state_photo_1 = 1;
    state_photo_2 = 1;
    ModeCount=7;
    if(M_SW ==0)
    {
      state=0;
      pixels.setPixelColor(0,0,0,0);
      pixels.setPixelColor(1,0,0,0);
      pixels.show();
      state_motor_1=0;
      state_motor_2=0;
      state_photo_1=0;
      state_photo_2=0;
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("SKIP!");
      delay(2000);
      ModeCount=0;
    }
  }
  Motor();
  Mode();
  Clock();
  compare();
  microsonic();
  
  t=rtc.getTime();
  Hour_t=t.hour;
  Min_t=t.min;
  Sec_t=t.sec;


      
      Serial.print(alarm_h_set);
      Serial.print(" : ");
      Serial.print(alarm_m_set);
      Serial.print(" : ");
      Serial.println(alarm_s_set);
     /* BTSerial.write(alarm_h_set);
      BTSerial.write(" : ");
      BTSerial.write(alarm_m_set);
      BTSerial.write(" : ");
      BTSerial.write(alarm_s_set);
      BTSerial.print(alarm_h_set);
      BTSerial.print(" : ");
      BTSerial.print(alarm_m_set);
      BTSerial.print(" : ");
      BTSerial.print(alarm_s_set);*/
      
      

     // sendPacket();
  
/************************디버그용***********************/
      //Serial.print(sta);
      //Serial.print(state_photo_1);
      //Serial.print(state_photo_2);
      //Serial.print(state_motor_1);
      //Serial.println(BT);

/*******************************************************/
            
}

void Motor()
{
  if(Serial.available())
  {
    BT=Serial.read();
  }
  C_SW=digitalRead(SW4);
  //state_motor_1 = 1일때
  if((BT=='1' || C_SW == 0) && state_motor_1 == 1)
  { 
    digitalWrite(EN_1,HIGH);
    digitalWrite(EN_2,LOW);
      
    //state=0;
    //ModeCount=0;
    //BT='0';
    //pixels.setPixelColor(0,0,0,0);
    //pixels.setPixelColor(1,0,0,0);
    //pixels.show();
  }
  if(digitalRead(photo_1) == 0)
    {
      state_photo_1 = 0;
    }
    //state_motor_1 = 으로 전환
    state_motor_1 = state_photo_1;


  //state_motor_1 = 0일때
  if(state_motor_1 == 0)
  {
    digitalWrite(EN_1,LOW);
    digitalWrite(EN_2,LOW);
  }
  
  //state_motor_2 = 1일때
  if((BT=='1' || C_SW == 0) && state_motor_2 == 1)
  {
    digitalWrite(EN_3,HIGH);
    digitalWrite(EN_4,LOW);
    
    state=0;
    ModeCount=0;
    BT='0';
    pixels.setPixelColor(0,0,0,0);
    pixels.setPixelColor(1,0,0,0);
    pixels.show();
  }
  if(digitalRead(photo_2) == 0)
    {
      state_photo_2 = 0;
    }
    //state_motor_2 =0 으로 전환
    state_motor_2 = state_photo_2;
    
  
  //state_motor_2 = 0일때
  if(state_motor_2 == 0)
  {
    digitalWrite(EN_3,LOW);
    digitalWrite(EN_4,LOW);
  }
}

void Mode()
{
  M_SW=digitalRead(SW1);
  if(M_SW==0)
  {
    ModeCount++;
    lcd.clear();
    Hour=Hour_t;
    Min=Min_t;
    delay(100);
  }
  if(ModeCount==6)
  {
    ModeCount=0;
  }
 
}

void Clock()
{
 
  if(LCD_Check==1 && ModeCount==0)//일반시계
  {
   LCD_Check = 0;
   lcd.clear();
   data();
   printTime();
  }
  if(ModeCount==1)//시계 시간설정
  {
   Set1();
   clockLCD();
   lcd.setCursor(0,0);
   lcd.print("Time_Hour Set");
  }
  if(ModeCount==2)//시계 분설정
  {
   Set2();
   clockLCD();
   lcd.setCursor(0,0);
   lcd.print("Time_Min Set");
  }
  if(ModeCount==3)//알람 시간설정
  {
   lcd.setCursor(0,0);
   lcd.print("Alarm_Hour Set");
   Set3();
   alarmLCD();
  }
   if(ModeCount==4)//알람 분설정
  {
   lcd.setCursor(0,0);
   lcd.print("Alarm_Min Set");
   Set4();
   alarmLCD();
  }
  if(ModeCount==5)//알람설정 시간 확인
  {
    set5();
  }
  if(ModeCount==7)//state=1일때
  {
    lcd.setCursor(0,0);
    lcd.print("Yellow: skip    ");
    lcd.setCursor(0,1);
    lcd.print("Green: discharge");
  }
}

void LCD()
{
  LCD_Check=1;
}

//시계 시간설정
 void Set1()
{
  M_SW=digitalRead(SW1);
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
  C_SW=digitalRead(SW4);
  
  if(U_SW==0 && D_SW==1)
  {
   //lcd.clear();
   Hour++;
   if(Hour>23){Hour=0;}
   delay(100);
  }
  else if(D_SW==0 && U_SW==1)
  {
   //lcd.clear();
   Hour--;
   if(Hour<0) {Hour=23;}
   delay(100);
  }
  if(C_SW==0)
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("CLOCK SET");
    lcd.setCursor(6,1);
    lcd.print("DONE");
    delay(1000);
    lcd.clear();
    rtc.setTime(Hour,Min,Sec);
  }
}

//시계 분설정
 void Set2()
{
  M_SW=digitalRead(SW1);
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
  C_SW=digitalRead(SW4);

  if(U_SW==0 && D_SW==1)
  {
    //lcd.clear();
    Min++;
    if(Min>59) {Min=0;}
    delay(100);
  }
  else if(D_SW==0 && U_SW==1)
  {
    //lcd.clear();
    Min--;
    if(Min<0) {Min=59;}
    delay(100);
  }
  if(C_SW==0)
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("CLOCK SET");
    lcd.setCursor(6,1);
    lcd.print("DONE");
    delay(1000);
    lcd.clear();
    rtc.setTime(Hour,Min,Sec);
  }
}

//알람 시간설정
void Set3()
{
  M_SW=digitalRead(SW1);
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
  C_SW=digitalRead(SW4);
    if(U_SW==0 && D_SW==1)
    {
      //lcd.clear();
      alarm_h2++;
      if(alarm_h2==10){alarm_h1++,alarm_h2=0;}
      if(alarm_h1==2 && alarm_h2==4) {alarm_h1=0, alarm_h2=0;}
      delay(100);
    }
     else if(D_SW==0 && U_SW==1)
     {
       //lcd.clear();
       alarm_h2--;
       if(alarm_h1==0)
      {
        if(alarm_h2<0)
        {
          alarm_h1=2;
          alarm_h2=3;
        }
      }
      else
      {
        if(alarm_h2<0)
        {
          alarm_h1--;
          alarm_h2=9;
        }
      }
      delay(100);
    }
    if(C_SW==0)
    {
      alarm_h_set = 10*alarm_h1+alarm_h2;
      alarm_m_set = 10*alarm_m1+alarm_m2;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ALARM SET");
      lcd.setCursor(6,1);
      lcd.print("DONE");
      delay(1000);
      lcd.clear();
    }
}

//알람 분설정
void Set4()
{

  M_SW=digitalRead(SW1);
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
  C_SW=digitalRead(SW4);
    if(U_SW==0 && D_SW==1)
    {
      //lcd.clear();
      alarm_m2++;
      if(alarm_m2==10){alarm_m1++,alarm_m2=0;}
      if(alarm_m1>5){alarm_m1=0,alarm_m2=0;}
      delay(100);
    }
     else if(D_SW==0 && U_SW==1)
     {
       //lcd.clear();
       alarm_m2--;
     if(alarm_m1==0)
      {
        if(alarm_m2<0)
        {
          alarm_m1=5;
          alarm_m2=9;
        }
      }
      else
      {
        if(alarm_m2<0)
        {
          alarm_m1--;
          alarm_m2=9;
        }
      }
      delay(100);
    }
    if(C_SW==0)
    {
      alarm_h_set = 10*alarm_h1+alarm_h2;
      alarm_m_set = 10*alarm_m1+alarm_m2;
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ALARM SET");
      lcd.setCursor(6,1);
      lcd.print("DONE");
      delay(1000);
      lcd.clear();
    }
}

void set5()
{
   lcd.setCursor(0,0);
   lcd.print("Check Alarm Set");
   lcd.setCursor(8,1);
   if(alarm_h_set<10) lcd.print("0");
   lcd.print(alarm_h_set);
   lcd.setCursor(10,1);
   lcd.print(":");
   lcd.setCursor(11,1);
   if(alarm_m_set<10) lcd.print("0");
   lcd.print(alarm_m_set);
   lcd.setCursor(13,1);
   lcd.print(":");
   lcd.setCursor(14,1);
   if(alarm_s_set<10) lcd.print("0");
   lcd.print(alarm_s_set);
}

void alarmLCD()
{
  lcd.setCursor(8,1);
  lcd.print(alarm_h1);
  lcd.setCursor(9,1);
  lcd.print(alarm_h2);
  lcd.setCursor(10,1);
  lcd.print(":");
  lcd.setCursor(11,1);
  lcd.print(alarm_m1);
  lcd.setCursor(12,1);
  lcd.print(alarm_m2);
  lcd.setCursor(13,1);
  lcd.print(":");
  lcd.setCursor(14,1);
  lcd.print(alarm_s1);
  lcd.setCursor(15,1);
  lcd.print(alarm_s2);
}

void clockLCD()
{
  lcd.setCursor(8,1);
  if(Hour<10) lcd.print("0");
  lcd.print(Hour);
  lcd.setCursor(10,1);
  lcd.print(":");
  lcd.setCursor(11,1);
  if(Min<10) lcd.print("0");
  lcd.print(Min);
  lcd.setCursor(13,1);
  lcd.print(":");
  lcd.setCursor(14,1);
  if(Sec<10) lcd.print("0");
  lcd.print(Sec);

}

void data()
{
  lcd.setCursor(0,1);
  lcd.print(rtc.getDOWStr()); //lcd에 요일 출력
  lcd.setCursor(3,0);
  lcd.print(rtc.getDateStr()); //lcd에 날짜 출력
}

void printTime()
{
  lcd.setCursor(8,1);
  lcd.print(rtc.getTimeStr());//시간 출력
}

void compare()
{
  if(Hour_t==alarm_h_set && Min_t==alarm_m_set && Sec_t == alarm_s_set)
  {
    state=1;
  }
}

void microsonic()
{
  
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
  duration = pulseIn(echo,HIGH);
  distance = (duration*340)/(1000*2);

  if(distance>150)
  {
      pixels.setPixelColor(2,10,0,0);
      pixels.setPixelColor(3,10,0,0);
      pixels.show();
  }
  else if(distance>100&&distance<150)
  {
      pixels.setPixelColor(2,10,0,10);
      pixels.setPixelColor(3,10,0,10);
      pixels.show();
  }
  else if(distance>50&&distance<100)
  {
      pixels.setPixelColor(2,2.6,4.2,10.2);
      pixels.setPixelColor(3,2.6,4.2,10.2);
      pixels.show();
  }
  else
  {
    pixels.setPixelColor(2,0,0,0);
      pixels.setPixelColor(3,0,0,0);
      pixels.show();
  }

}
/*
void sendPacket()
{
  byte data[6] = {0,};

  data[0] = 0xF0;
  data[1] = Serial.print(alarm_h_set);
  data[2] = Serial.print(alarm_m_set);
  data[3] = Serial.print(alarm_s_set);
  data[4] = state;
  data[5] = 0xF1;

  Serial.write(data, 6);
}*/
