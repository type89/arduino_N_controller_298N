#include <L298N.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

const unsigned int IN1 = 10;
const unsigned int IN2 = 9;
const unsigned int ENA = 11;
#define SW 4

L298N motor(ENA, IN1, IN2);
LiquidCrystal_I2C lcd(0x27,16,2);
Encoder myEnc(2, 3);
 

typedef struct { 
  int num;
  char* notch;
  int duty;
  char* arrow;
} dataDictionary;

const dataDictionary myDictionaryArr[] {
    {0, "L-16",130, "<======="},
    {1, "L-15",120, "(======="},
    {2, "L-14",110, " <======"},
    {3, "L-13",100, " (======"},
    {4, "L-12",90,  "  <====="},
    {5, "L-11",85,  "  (====="},
    {6, "L-10",80,  "   <===="},
    {7, "L-9", 75,  "   (===="},
    {8, "L-8", 70,  "    <==="},
    {9, "L-7", 65,  "    (==="},
    {10,"L-6", 60,  "     <=="},
    {11,"L-5", 55,  "     (=="},
    {12,"L-4", 50,  "      <="},
    {13,"L-3", 45,  "      (="},
    {14,"L-2", 40,  "       <"},
    {15,"L-1", 35,  "       ("},
    {16,"STOP",15,  "      ####"},
    {17,"R-1", 35,  "        )"},
    {18,"R-2", 40,  "        >"},
    {19,"R-3", 45,  "        =)"},
    {20,"R-4", 50,  "        =>"},
    {21,"R-5", 55,  "        ==)"},
    {22,"R-6", 60,  "        ==>"},
    {23,"R-7", 65 , "        ===)"},
    {24,"R-8", 70 , "        ===>"},
    {25,"R-9", 75,  "        ====)"},
    {26,"R-10",80,  "        ====>"},
    {27,"R-11",85,  "        =====)"},
    {28,"R-12",90,  "        =====>"},
    {29,"R-13",100, "        ======)"},
    {30,"R-14",110, "        ======>"},
    {31,"R-15",120, "        =======)"},
    {32,"R-16",130, "        =======>"},
};


long oldPosition  = -999;
float num = 15.5;
boolean up_num = true; 
int lastButtonPress = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial)
  {
  }
  motor.setSpeed(15);
  Serial.println("Basic Encoder Test:");
  pinMode(SW, INPUT_PULLUP); 
  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("*Arduino");
  lcd.setCursor(0, 1);
  lcd.print("*N-Controller 2");
  delay(3000);
}


void loop() {

  int btnState = digitalRead(SW);
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      num = 16;
      set_lcd();
      motor_set_duty(int(num));
      Serial.print("press_notch ==> " );
      Serial.println(num);
      
     }
    lastButtonPress = millis();
    Serial.print("lastButtonPress = ==> " );
    Serial.println(lastButtonPress);
  }
  
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    if (oldPosition > newPosition){
      up_num = false; 
      num = num -0.5;
      if (num < -0.5){
        num = 0;
      }
    }
    if (oldPosition < newPosition){
      up_num = true; 
      num = num + 0.5;
      if (num > 32.5){
        num = 32;
      }
    }
    float num_10 = num *10;
    if(((int)num_10 % 10) == 0){
      set_lcd();
      motor_set_duty(int(num));
    }
    oldPosition = newPosition;
    //Serial.println(newPosition);
  }
}

void set_lcd(){
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Notch :: ");
  lcd.setCursor(11, 0);
  lcd.print(myDictionaryArr[int(num)].notch);
  lcd.setCursor(0, 1);
  //lcd.print(myDictionaryArr[int(num)].duty);
  lcd.print(myDictionaryArr[int(num)].arrow);
}

void motor_set_duty(int num){
  if(num > 16){
    motor.forward();
    if(up_num){
      if(num == 17){
        motor.setSpeed(60);
        delay(5);
      }
      increase_duty(myDictionaryArr[int(num-1)].duty, myDictionaryArr[int(num)].duty);
    }else{
      decrease_duty(myDictionaryArr[int(num)].duty, myDictionaryArr[int(num-1)].duty);
    }  
  }
  if(num == 16){
    motor.stop();
    if(up_num){
      motor.forward();
     }else{
      motor.backward();
    }
    motor.setSpeed(myDictionaryArr[int(num)].duty);
  }
  if(num < 16){
    motor.backward();
    if(up_num){
      decrease_duty(myDictionaryArr[int(num-1)].duty, myDictionaryArr[int(num)].duty); 
    }else{
      if(num == 15){
        motor.setSpeed(60);
        delay(5);
      }
      increase_duty(myDictionaryArr[int(num+1)].duty, myDictionaryArr[int(num)].duty);
    }  
  }
  Serial.print("notch ==> " );  
  Serial.println(myDictionaryArr[int(num)].notch); 
}

void increase_duty(int fromduty, int toduty){
  Serial.println("increase " ); 
  Serial.print("fromduty ==> " ); 
  Serial.println(fromduty);
  Serial.print("toduty ==> " ); 
  Serial.println(toduty);
  for(int duty = fromduty; duty <= toduty; duty++){
    Serial.print("duty ==> " );  
    Serial.println(duty); 
    delay(30);
    motor.setSpeed(duty); 
  }
}

void decrease_duty(int fromduty, int toduty){
  Serial.println("decrease " ); 
  Serial.print("fromduty ==> " ); 
  Serial.println(fromduty);
  Serial.print("toduty ==> " ); 
  Serial.println(toduty);
  for(int duty = fromduty; duty >= toduty; duty--){
    Serial.print("duty ==> " );  
    Serial.println(duty); 
    delay(30);
    motor.setSpeed(duty);  
  }
}
