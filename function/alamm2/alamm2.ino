#include<LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27,16,2);

int starttime;
int activetime;
int prevoustime = 0;

int hours = 0;
int mins = 0;

int ahours = 0;
int amins = 0;

void setup()
{
  lcd.begin();
  lcd.clear();
 
  Serial.begin(9600);
 
  pinMode(12, INPUT);
  digitalWrite(12, HIGH);
  pinMode(11, INPUT);
  digitalWrite(11, HIGH);
  pinMode(10, INPUT);
  digitalWrite(10, HIGH);
 
   pinMode(13, INPUT);
  digitalWrite(13, HIGH);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
 
   pinMode(9, OUTPUT);
 
  starttime = millis()/1000;
}

void loop()
{
  while(digitalRead(13) == LOW)
  {
    lcd.setCursor(6,1);
    lcd.print("Alarm");
    lcd.setCursor(6,0);
       if(digitalRead(11) == LOW)
    {
     amins++;
    }
    else if (digitalRead(10) == LOW)
    {
      ahours++;
    }
     lcd.setCursor(6,0);
 
  if(ahours < 10)
  {
    lcd.print("0");
    lcd.print(ahours);
  }
  else
  {
    lcd.print(ahours);
  }
   
    lcd.print(":");
   
  if (amins < 10)
  {
    lcd.print("0");
      lcd.print(amins);
  }
  else
  {
      lcd.print(amins);
  }
 if(amins > 59)
     {
      ahours++;
      amins = 0;
     }
     if(ahours > 23)
     {
      ahours = 0;
     }
     delay(500);
     lcd.clear();
  }
 
  if(digitalRead(12) == LOW)
  {
   
    lcd.setCursor(5,1);
    lcd.print("Set Time");
    lcd.setCursor(6,0);
     if(digitalRead(11) == LOW)
    {
     mins++;
    }
    else if (digitalRead(10) == LOW)
    {
      hours++;
    }
   

  }
 
      activetime = (millis() / 1000) - starttime;
      if(prevoustime < (activetime - 59))
      {
       mins++;
       prevoustime = activetime;
      }
     
      if(mins > 59)
     {
      hours++;
      mins = 0;
     }
    
     if(hours > 23)
     {
      hours = 0;
     }
 
 
  lcd.setCursor(6,0);
 
  if(hours < 10)
  {
    lcd.print("0");
    lcd.print(hours);
  }
  else
  {
    lcd.print(hours);
  }
   
    lcd.print(":");
   
  if (mins < 10)
  {
    lcd.print("0");
      lcd.print(mins);
  }
  else
  {
      lcd.print(mins);
  }

 

if(ahours == hours && amins == mins && amins != 0)
{
  tone(9, 1000, 200);
  delay(200);
  noTone(9);
delay(200);
}
else
{
    delay(300);
}
  lcd.clear();
 

  Serial.println(mins);
  Serial.println(hours);
  Serial.println("");
  Serial.println(amins);
  Serial.println(ahours);
  Serial.println("");
  Serial.println(activetime);
  Serial.println(prevoustime);
  Serial.println(starttime);
  Serial.println("");
}
