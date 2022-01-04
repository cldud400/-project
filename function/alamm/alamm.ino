#include<LiquidCrystal_I2C.h>
#include<DS1302.h>
#include<Wire.h>
DS1302 rtc(2,3,4);
Time t;
LiquidCrystal_I2C lcd(0x27,16,2);
void serial(void);
void LCD(void);
int TEMP = 0;
int M_SW=0;
int ModeCount=0;
int U_SW, D_SW;
int Hour,Min,Sec;
int piezo = 12;
int note = 2793;
int alamh1, alamh2,alamm1,alamm2,alams1,alams2=0;
int alamh=alamh1*10*alamh2;
int alamm=alamm1*10*alamm2;
int alams=alams1*10*alams2;

void Mode(){ //모드 설정 함수
  M_SW=digitalRead(10);
  if(M_SW==0){
    ModeCount++;
  }
  if(ModeCount==0){
    //noTone(piezo);
    HourEDIT();
    LCD(); //모드 0 = 일반 시계모드
  }
  else if (ModeCount==1){
    HourEDIT();
    LCD(); //모드 1 = 분 변경모드
  }
  else if(ModeCount==2){
    MinEDIT(); //모드 2 = 시 변경모드
    LCD();
  }
  else if(ModeCount==3){
    AlamhEDIT(); //모드 3 = 알람 시 변경모드
    AlamLCD();
  }
   else if(ModeCount==4){
    AlammEDIT(); //모드 4 = 알람 분 변경모드
    AlamLCD();
    rtc.halt(false);
  }
   else if(ModeCount==5){
   PIEZO();
   Alammode_LCD(); //모드 5 = 알람 시계 모드
  }
   else if(ModeCount==6){
   ModeCount=0; //시계모드
  }
}

void AlamLCD(){
  rtc.halt(true);
  lcd.clear();
  lcd.print("TIME : ");
  if(alamh < 10){
    lcd.print("0");
  }
  lcd.print(alamh);
  lcd.print(":");
  if(alamm <10){
    lcd.print("0");
  }
  lcd.print(alamm);
  lcd.print(":");
  if(Sec <10){
    lcd.print("0");
  }
  lcd.print(alams);
  lcd.setCursor(0,1);
  lcd.print("AlamEdit");
  lcd.setCursor(11,1);
  lcd.print("MODE");
  lcd.print(ModeCount);
}

void Alammode_LCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  if(Hour <10){
    lcd.print("0");
  }
  lcd.print(Hour,DEC);
  lcd.print(":");
  if(Min <10){
    lcd.print("0");
  }
  lcd.print(Min,DEC);
  lcd.print(":");
  if(Sec <10){
    lcd.print("0");
  }
  lcd.print(Sec,DEC);
  lcd.setCursor(10,0);
  //lcd.print(TEMP);
  lcd.write(byte(0));
  lcd.setCursor(0,1);
  lcd.print("ALAMMODE");
  if(alamh<10){
    lcd.print("0");
  }
  lcd.print(alamh);
  lcd.print(":");
  if(alamm<10){
    lcd.print("0");
  }
  lcd.print(alamm);
}
void serial(){
  Serial.print(t.hour, DEC);
  Serial.print(":");
  Serial.print(t.min,DEC);
  Serial.print(":");
  Serial.println(t.sec,DEC);
}
void LCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TIME : ");
  if(t.hour<10){
    lcd.print("0");
  }
  lcd.print(Hour, DEC);
  lcd.print(":");
  if(t.min<10){
    lcd.print("0");
  }
  lcd.print(Min, DEC);
  lcd.print(":");
  if(t.sec<10){
    lcd.print("0");
  }
  lcd.print(Sec, DEC);
  lcd.setCursor(0,1);
  //lcd.print("TEMP : ");
  //lcd.print(TEMP);
  //lcd.write(byte(0));
}

void HourEDIT(){
  rtc.halt(true);
  U_SW=digitalRead(11);
  D_SW=digitalRead(12);
    if(U_SW==0 && D_SW==1){
      Hour++;
    }
    else if(D_SW==1 && U_SW==0){
      Hour--;
      if(Hour<0){Hour=0;}
    }
    rtc.setTime(Hour,Min,Sec);
    lcd.setCursor(0,2);
    lcd.print("MODE1");
}
void MinEDIT(){
  rtc.halt(true);
  U_SW=digitalRead(11);
  D_SW=digitalRead(12);
    if(U_SW==0 && D_SW==1){
      Min++;
    }
    else if(D_SW==0 && U_SW==1){
      Min--;
      if(Min<0){Min=0;}
    }
    rtc.setTime(Hour,Min,Sec);
}

void AlamhEDIT(){
  lcd.clear();
  U_SW=digitalRead(11);
  D_SW=digitalRead(12);
    if(U_SW==0 && D_SW==1){
      alamh++;
      if(alamh>23){alamh=23;}
    }
    else if(D_SW==0 && U_SW==1){
      alamh--;
      if(alamh<0){alamh=0;}
    }
}

void AlammEDIT(){
  lcd.clear();
  U_SW=digitalRead(11);
  D_SW=digitalRead(12);
    if(U_SW==0 && D_SW==1){
      alamm++;
      if(alamm>59){alamm=59;}
    }
    else if(D_SW==0 && U_SW==1){
      alamh--;
      if(alamm<0){alamm=0;}
    }
}

void PIEZO(){
  if(Hour==alamh && Min==alamm && Sec==alams){
    tone(piezo,523);
  }
}

void setup() {
 Serial.begin(9600); 
 //rtc.setTime(15,13,00);
 rtc.halt(false);
 rtc.writeProtect(false);
 lcd.begin();
 pinMode(10,INPUT_PULLUP);
 pinMode(11,INPUT_PULLUP);
 pinMode(12,INPUT_PULLUP);

}

void loop() {
  //TEMP = dht.readTemperature();
  t=rtc.getTime();
  Hour=t.hour;
  Min=t.min;
  Sec=t.sec;
  Mode();
  serial();
  LCD();
  delay(1000);
  Serial.print("state_photo_1 : "); Serial.println(M_SW);
  Serial.print("state_photo_2 : "); Serial.println(U_SW);
  Serial.print("state_photo_3 : "); Serial.println(D_SW);
  Serial.print("state_photo_4 : "); Serial.println(ModeCount);

}
