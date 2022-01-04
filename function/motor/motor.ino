#define EN1 8
#define EN2 7
#define EN3 6
#define EN4 5

char in_data;
void setup() {
  Serial.begin(9600);
  pinMode(EN1, OUTPUT);       // Motor A 방향설정1

  pinMode(EN2, OUTPUT);       // Motor A 방향설정2

  pinMode(EN3, OUTPUT);       // Motor B 방향설정1

  pinMode(EN4, OUTPUT);       // Motor B 방향설정2


}



void loop() {

  if(Serial.available())
  {  
    in_data=Serial.read();
    switch(in_data){
      
    case '1' :
      digitalWrite(EN1,HIGH);
      digitalWrite(EN2,LOW);
      digitalWrite(EN3,HIGH);
      digitalWrite(EN4,LOW);
      Serial.println("MOTOR ON");
      Serial.println();
      break;
    case '2' :
      digitalWrite(EN1,LOW);
      digitalWrite(EN2,LOW);
      digitalWrite(EN3,LOW);
      digitalWrite(EN4,LOW);
      Serial.println("MOTOR STOP");
      Serial.println();
      break;
    }
   }
  }
