#include <DS1302.h>


#include <DS1302.h>          // DS1302 RTC 시계모듈 라이브러리
#include <Wire.h>           // wire 라이브러리 (lcd통신)
#include <LiquidCrystal_I2C.h>  // LCD 라이브러리

DS1302 rtc(2,3,4); //rtc 객체(연결된 핀정보 RST, DAT CLK)

//0x3f I2C주소를 가지고 있는 16x2 lcd객체 생성 (i2c 주소는 lcd에 맞게 수정)
LiquidCrystal_I2C lcd(0x27,16,2);


int photo_1 = A0;
int photo_2 = A1;
int EN_1 = 8;
int EN_2 = 7;
int EN_3 = 6;
int EN_4 = 5;

int state=0;         //동작 상태 변수 0:평시,1:작동
int state_motor_1=0; //모터 1 상태
int state_motor_2=0; //모터 2 상태
int state_photo_1=0; //포토 인터럽터 1 상태
int state_photo_2=0; //포토 인터럽터 2 상태
void setup()
{
  Serial.begin(9600); //시리얼 통신 시작
  
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
  //rtc.setTime(14,39,00); //시간설정 시,분,초
  //rtc.setDate(22,05,2020); //날짜설정 일 월 년
  
}

void loop()
{
  //시간이 되면 state_motor, state_photo를 1로 전환
  if(Serial.read()=='2') //추후변경
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
  if(state_motor_1==0)
  {
    digitalWrite(EN_1,LOW);
    digitalWrite(EN_2,LOW);
  }


  //state_motor_2 = 1일때
  if(state_motor_2==1)
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
  if(state_motor_2==0)
  {
    digitalWrite(EN_3,LOW);
    digitalWrite(EN_4,LOW);
  }
    
      Serial.println(digitalRead(photo_1));
      Serial.println(digitalRead(photo_2));
      Serial.print("\n");
    
      Serial.print("state_photo_1 : "); Serial.println(state_photo_1);
      Serial.print("state_photo_2 : "); Serial.println(state_photo_2);
      Serial.print("state_motor_1 : "); Serial.println(state_motor_1);
      Serial.print("state_motor_2 : "); Serial.println(state_motor_2); 
  
      Serial.print(rtc.getDOWStr()); // 요일 시리얼 모니터에 출력
      Serial.print(" ");
      Serial.print(rtc.getDateStr());
      Serial.print(" "); // 날짜를 시리얼 모니터에 출력
      Serial.println(rtc.getTimeStr()); // 시간을 시리얼 모니터에 출력
      lcd.clear();
      data();
      printTime();
      delay(1000);
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
