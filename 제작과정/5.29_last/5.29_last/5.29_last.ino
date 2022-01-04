
#include <DS1302.h>          // DS1302 RTC 시계모듈 라이브러리
#include <Wire.h>           // wire 라이브러리 (lcd통신)
#include <LiquidCrystal_I2C.h>  // LCD 라이브러리
#include <TimerOne.h>

DS1302 rtc(2,3,4); //rtc 객체(연결된 핀정보 RST, IO, SCL)

//0x3f I2C주소를 가지고 있는 16x2 lcd객체 생성 (i2c 주소는 lcd에 맞게 수정)
LiquidCrystal_I2C lcd(0x27,16,2);


int photo_1 = A0;
int photo_2 = A1;
//모터 핀 지정
int EN_1 = 8;
int EN_2 = 7;
int EN_3 = 6;
int EN_4 = 5;
//스위치 핀 지정
int SW1 = 10;
int SW2 = 11;
int SW3 = 12;
int SW4 = 13;
int Hour,Min,Sec;
int ModeCount=0;
int M_SW, U_SW, D_SW;

int alarm_h1,alarm_h2,alarm_m1,alarm_m2,alarm_s1,alarm_s2;
int alarm_h;
int alarm_m;
int alarm_s;

String CurrentTime;
Time t;
int Hour_1,Min_2,Sec_3;

int state=0;         //동작 상태 변수 0:평시,1:작동
int state_motor_1=0; //모터 1 상태
int state_motor_2=0; //모터 2 상태
int state_photo_1=0; //포토 인터럽터 1 상태
int state_photo_2=0; //포토 인터럽터 2 상태

volatile bool LCD_Done =0;

void setup()
{
  Serial.begin(9600); //시리얼 통신 시작
  Timer1.initialize(1000000);
  Timer1.attachInterrupt(LCD);

  
  //포토인터럽터
  pinMode(photo_1,INPUT);
  pinMode(photo_2,INPUT);
  
  //모터
  pinMode(EN_1, OUTPUT);       // Motor A 방향설정1
  pinMode(EN_2, OUTPUT);       // Motor A 방향설정2
  pinMode(EN_3, OUTPUT);       // Motor B 방향설정1
  pinMode(EN_4, OUTPUT);       // Motor B 방향설정2
  
  //rtc모듈
  rtc.halt(false);
  rtc.writeProtect(false);

  //lcd 시작
  lcd.begin();         //i2c lcd 초기화
  lcd.backlight();    //백라이트 on

  //* 시간설정 * 한번만업로드하고 주석처리후 재업로드
  //rtc.setDOW(FRIDAY); //요일설정
  //rtc.setTime(16,39,30); //시간설정 시,분,초
  //rtc.setDate(29,05,2020); //날짜설정 일 월 년
  
}

void loop()
{
  //시간이 되면 state_motor, state_photo를 1로 전환
  if(state == 1) //추후변경
  {
    state_motor_1 = 1;
    state_motor_2 = 1;
    state_photo_1 = 1;
    state_photo_2 = 1;
  }

  //state_motor_1 = 1일때
  if(state_motor_1 == 1)
  { 
    digitalWrite(EN_1,HIGH);
    digitalWrite(EN_2,LOW);
      
    if(digitalRead(photo_1) == 0)
    {
      state_photo_1 = 0;
    }
    //state_motor_1 = 으로 전환
    state_motor_1 = state_photo_1;
  }


  //state_motor_1 = 0일때
  if(state_motor_1 == 0)
  {
    digitalWrite(EN_1,LOW);
    digitalWrite(EN_2,LOW);
  }
  
  //state_motor_2 = 1일때
  if(state_motor_2 == 1)
  {
    digitalWrite(EN_3,HIGH);
    digitalWrite(EN_4,LOW);

    if(digitalRead(photo_2) == 0)
    {
      state_photo_2 = 0;
    }
    //state_motor_2 =0 으로 전환
    state_motor_2 = state_photo_2;
  }
  
  //state_motor_2 = 0일때
  if(state_motor_2 == 0)
  {
    digitalWrite(EN_3,LOW);
    digitalWrite(EN_4,LOW);
  }

  if(state_motor_1==0 && state_motor_2==0) state=0;
  
  if(LCD_Done==1 && ModeCount==0)
  {
   LCD_Done = 0;
   lcd.clear();
   data();
   printTime();
  }
  if(ModeCount==1)
  {
  lcd.setCursor(0,0);
  lcd.print("Hour Mode");
   printTime();
   LCD2();
   delay(100);
  }
  if(ModeCount==2)
  {
   lcd.setCursor(0,0);
   lcd.print("Min  Mode");
   printTime();
   LCD3();
   delay(100);
  }
  if(ModeCount==3)
  {
   lcd.setCursor(0,0);
   lcd.print("Alarm_H  Mode");
   alarm();
   LCD4();
   delay(100);
  }
   if(ModeCount==4)
  {
   lcd.setCursor(0,0);
   lcd.print("Alarm_M  Mode");
   alarm();
   LCD5();
   delay(100);
  }
  Mode();
  compare();
  t=rtc.getTime();
  Hour_1=t.hour;
  Min_2=t.min;
  Sec_3=t.sec;


   alarm_h=10*alarm_h1+alarm_h2;
   alarm_m=10*alarm_m1+alarm_m2;
   alarm_s=10*alarm_s1+alarm_s2;



      
      //디버그용
      /*Serial.print(rtc.getDOWStr()); // 요일 시리얼 모니터에 출력
      Serial.print(" ");
      Serial.print(rtc.getDateStr());
      Serial.print(" "); // 날짜를 시리얼 모니터에 출력
      Serial.println(rtc.getTimeStr()); // 시간을 시리얼 모니터에 출력

      Serial.print("state_photo_1 : "); Serial.println(state_photo_1);
      Serial.print("state_photo_2 : "); Serial.println(state_photo_2);
      Serial.print("state_motor_1 : "); Serial.println(state_motor_1);
      Serial.print("state_motor_2 : "); Serial.println(state_motor_2);*/
      /*Serial.print("SW1 : ");
      Serial.println(digitalRead(SW1));
      Serial.print("SW2 : ");
      Serial.println(digitalRead(SW2));
      Serial.print("SW3 : ");
      Serial.println(digitalRead(SW3));
      Serial.print("MOdeCount : ");
      Serial.println(ModeCount);*/

      Serial.print("Hour : ");
      Serial.println(Hour_1);
      Serial.print("Min : ");
      Serial.println(Min_2);
      Serial.print("state : ");
      Serial.println(state);
      Serial.print("Alarm_h : ");
      Serial.println(alarm_h);
      Serial.print("Alarm_m");
      Serial.println(alarm_m);
     
        
      
}
/*      
      Serial.println(digitalRead(photo_1));
      Serial.println(digitalRead(photo_2));
      Serial.print("\n");
  
      Serial.print(rtc.getDOWStr()); // 요일 시리얼 모니터에 출력
      Serial.print(" ");
      Serial.print(rtc.getDateStr());
      Serial.print(" "); // 날짜를 시리얼 모니터에 출력
      Serial.println(rtc.getTimeStr()); // 시간을 시리얼 모니터에 출력

      lcd.clear();
      data();
      printTime();
      delay(1000);
*/

void data()
{
  lcd.setCursor(0,1); //커서 위치
  lcd.print(rtc.getDOWStr()); //lcd에 요일 출력
  lcd.setCursor(3,0);
  lcd.print(rtc.getDateStr()); //lcd에 날짜 출력
}

void printTime()
{
  lcd.setCursor(8,1);
  lcd.print(rtc.getTimeStr());
}
void LCD()
{
  LCD_Done=1;
}


void Mode(){ //모드 설정 함수
  M_SW=digitalRead(SW1);
  if(M_SW==0){
    ModeCount++;
    lcd.clear();
    delay(100);
  }
  if(ModeCount==1){
    LCD2();
    delay(100);
  }
  else if(ModeCount==2){
    LCD3();
    delay(100);
  }
    else if(ModeCount==3){
      LCD4();
    delay(100);
  }
    else if(ModeCount==4){
      LCD5();
    delay(100);
  }
  else if(ModeCount==5){
    delay(100);
  }
  if(ModeCount==6){
    ModeCount=0;
  }
 
}
 void LCD2()
{
  //rtc.halt(true);
  
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
    if(U_SW==0 && D_SW==1){
      //lcd.clear();
      Hour++;
      if(Hour>23){Hour=0;}
    }
    else if(D_SW==0 && U_SW==1){
      //lcd.clear();
      Hour--;
      if(Hour<0){Hour=23;}
    }
    rtc.setTime(Hour,Min,Sec);
}
 void LCD3()
{
  //rtc.halt(true);
  
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
    if(U_SW==0 && D_SW==1){
      //lcd.clear();
      Min++;
      if(Min>59){Min=0;}
    }
    else if(D_SW==0 && U_SW==1){
      //lcd.clear();
      Min--;
      if(Min<0){Min=59;}
    }
    rtc.setTime(Hour,Min,Sec);
}

void LCD4()
{
  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
    if(U_SW==0 && D_SW==1){
      //lcd.clear();
      alarm_h2++;
      if(alarm_h2==10){alarm_h1++,alarm_h2=0;}
      if(alarm_h1==2 && alarm_h2==4){alarm_h1=0,alarm_h2=0;}
    }
     else if(D_SW==0 && U_SW==1){
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
    }
  
}
void LCD5()
{

  U_SW=digitalRead(SW2);
  D_SW=digitalRead(SW3);
    if(U_SW==0 && D_SW==1){
      //lcd.clear();
      alarm_m2++;
      if(alarm_m2==10){alarm_m1++,alarm_m2=0;}
      if(alarm_m1>5){alarm_m1=0,alarm_m2=0;}
    }
     else if(D_SW==0 && U_SW==1){
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
    }
}
void alarm()
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
void compare()
{
  if(Hour_1==alarm_h && Min_2==alarm_m)
  {
    state=1;
  }
}
