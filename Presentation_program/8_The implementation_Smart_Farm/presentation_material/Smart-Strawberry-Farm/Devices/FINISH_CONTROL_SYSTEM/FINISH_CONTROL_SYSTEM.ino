      #include <LiquidCrystal.h>
      #include<SoftwareSerial.h>
      #include "DHT.h"
      #include <Wire.h>
      #include <Keypad.h>
      #include <Password.h>   
      
      //LCD
      const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
      LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

      String MyinputString = "";
      char inChar;

      //DHT
      char inChar1;
      int Temp, Humi,Temp2,Humi2;
      #define DHTPIN A0
      #define DHTTYPE DHT22
      int AVG_TEMP;
      int AVG_HUMI;

      //FROM NODE_MCU
      int humi;
      int temp;
      int Hour;
      int Minutes;
      String PUMP;

      //SOIL
      #define SOILPin A3
      #define SOILPin1 A4
      int Sumsoil;
      
      //LDR
      int LDRPin = A2; 
      int LDRValue = 0; 
      
      //Relay
      #define COOLER_PUMP 26
      #define WATER_PUMP 27
      #define COOLER 28
      #define FAN_IN_OUT 29
      
      //Buzzer
      #define Buzzer 22
    
      // CONDITION HIGH
      int TempHigh = 30;
      int HumiHigh = 80;
      
      //KEYPAD
      int STATE = HIGH;
      int locked = 1; 
      int passinput = 0;
      int lockedled = 14;
      int unlockedled = 15;
      long ledflashvar = 0;
      long ledflashtime = 300;  
      const byte ROWS = 4;
      const byte COLS = 4;
      char key;
      char keys[ROWS][COLS] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
      byte rowPins[ROWS] = {46, 47, 48, 49};
      byte colPins[COLS] = {50, 51, 52, 53};
      Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
      Password password = Password("0000");   
      boolean OFF =  HIGH;
      boolean ON = LOW;
        
      DHT dht(DHTPIN, DHTTYPE);
      
void setup() {
      SET_SENSOR();
      SET_LCD();
      SET_SERIAL();
      SET_RELAY();
      SET_BUZZER();
      SETFIRST_RELAY();
      delay(1000);
}

void loop() {
                KEYPAD_CONTROL();
                Get_DHT();
                Get_Soil();
                Get_LDR();
                delay(50);  
                 if (key != NO_KEY){
                      Controller();   
                 }  
                 if(key == NO_KEY){
                    //AUTO PROGRAM  
                    if(AVG_HUMI >= HumiHigh && AVG_TEMP > TempHigh ){
                            digitalWrite(FAN_IN_OUT,ON);
                            digitalWrite(WATER_PUMP,OFF);
                            
                           // digitalWrite(COOLER_PUMP, ON);
                    }
                    else if(AVG_HUMI >= HumiHigh  && AVG_TEMP <= TempHigh ){
                             digitalWrite(FAN_IN_OUT,ON);//TEST
                           //  digitalWrite(COOLER_PUMP, OFF);
                           //  digitalWrite(WATER_PUMP,OFF);
                            // digitalWrite(COOLER,ON);       
                    }
                    else if(AVG_HUMI <= HumiHigh && AVG_TEMP > TempHigh){
                            digitalWrite(FAN_IN_OUT,LOW);
                            Serial.print("DO WORK");
                           // digitalWrite(COOLER_PUMP, ON);
                           // digitalWrite(WATER_PUMP,OFF);
                            //digitalWrite(COOLER,ON);
                    }
                    else if(AVG_HUMI <= HumiHigh && AVG_TEMP <= TempHigh){
                            digitalWrite(FAN_IN_OUT,OFF);
                            //digitalWrite(COOLER_PUMP, OFF);
                            //digitalWrite(WATER_PUMP,OFF);
                            //digitalWrite(COOLER,ON);
                    }else{
                          digitalWrite(FAN_IN_OUT,OFF);
                          digitalWrite(COOLER_PUMP, OFF);
                          digitalWrite(WATER_PUMP,OFF);
                          digitalWrite(COOLER,ON);
                    }
                 }    
}
void Get_DHT(){ 
      Humi = dht.readHumidity();
      Temp = dht.readTemperature();
      if (isnan(Humi) || isnan(Temp) || isnan(Humi2) || isnan(Temp2) ) {
        //Serial.println("Failed to read from DHT sensor!");
        return;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("T: ");
      lcd.print(Temp);
      lcd.print((char)223);
      lcd.print("C");
      lcd.print(" ");
      lcd.print(" H: ");
      lcd.print(Humi);
      lcd.print(" %"); 
 }
void Get_Soil(){
      int sensorValue;
      sensorValue = analogRead(SOILPin);
      sensorValue = sensorValue - 500;
      lcd.setCursor(0,1);
      lcd.print("S: ");
      lcd.print(sensorValue);
      lcd.print(" ");
      lcd.print(" ");
      lcd.print("L: ");
      lcd.print(LDRValue);
}
void Get_LDR(){
      LDRValue = analogRead(LDRPin); 
}
void SET_RELAY(){
      pinMode(COOLER_PUMP,OUTPUT);
      pinMode(WATER_PUMP,OUTPUT);
      pinMode(COOLER,OUTPUT);
      pinMode(FAN_IN_OUT,OUTPUT);
      digitalWrite(COOLER,HIGH);
}
void SET_SERIAL(){
      Serial.begin(115200);
      //Serial.print(char(169));
}
void SET_LCD(){
      lcd.begin(16, 2);
      lcd.setCursor(0,0);
      lcd.print("     WELCOME    ");
      lcd.setCursor(0,1);
      lcd.print("   STRAWBERRY   ");
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("System Boot ");
      delay(200);
      lcd.print(".");
      delay(600);
      lcd.print(".");
      delay(500);
      lcd.print(".");
      delay(500);
      lcd.print(".");
      delay(500);
      lcd.clear();
}
void SET_SENSOR(){
      dht.begin();
      pinMode(SOILPin,INPUT);
      pinMode(SOILPin1,INPUT);
      pinMode(LDRPin,INPUT);
}
void SET_BUZZER(){
      pinMode(Buzzer,OUTPUT);
}
void KEYPAD_CONTROL(){
      key = keypad.getKey();
}
void SETFIRST_RELAY(){
      digitalWrite(WATER_PUMP,OFF);
      digitalWrite(COOLER_PUMP,OFF);
      digitalWrite(COOLER,ON);
      digitalWrite(FAN_IN_OUT,OFF);
}
void SET_TEMP(){
        key = keypad.getKey();
        //Serial.println(key);
        lcd.clear();   
        lcd.setCursor(0,0);
        lcd.print("  SET TempHigh  ");
        lcd.setCursor(0,1);
        lcd.print(" TempHigh = ");
        while(key != '*'){
          //Serial.print("ENTER 5");
              key = keypad.getKey();
              //Serial.println(key);
                  if(key=='3'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 3");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET TempHigh  ");
                    lcd.setCursor(0,1);
                    lcd.print(" TempHigh = ");
                    lcd.print("3");     
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                     TempHigh = 30;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '1':
                                       TempHigh = 31;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                      case '2':
                                       TempHigh = 32;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '3':
                                        TempHigh = 33;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '4':
                                        TempHigh = 34;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '5':
                                        TempHigh = 35;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '6':
                                        TempHigh = 36;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '7':
                                        TempHigh = 37;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '8':
                                        TempHigh = 38;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                    case '9':
                                        TempHigh = 39;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                  }
                    }
               }
                if(key=='2'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 2");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET TempHigh  ");
                    lcd.setCursor(0,1);
                    lcd.print(" TempHigh = ");
                    lcd.print("2");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                      TempHigh = 20;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '1':
                                        TempHigh = 21;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                      case '2':
                                        TempHigh = 22;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '3':
                                        TempHigh = 23;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '4':
                                        TempHigh = 24;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '5':
                                        TempHigh = 25;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '6':
                                        TempHigh = 26;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '7':
                                        TempHigh = 27;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '8':
                                        TempHigh = 28;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                    case '9':
                                        TempHigh = 29;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                  }
                    }
               }
               if(key=='1'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 1");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET TempHigh  ");
                    lcd.setCursor(0,1);
                    lcd.print(" TempHigh = ");
                    lcd.print("1");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                      TempHigh = 10;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '1':
                                        TempHigh = 11;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                      case '2':
                                        TempHigh = 12;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '3':
                                        TempHigh = 13;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '4':
                                        TempHigh = 14;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '5':
                                        TempHigh = 15;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '6':
                                        TempHigh = 16;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '7':
                                        TempHigh = 17;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                     case '8':
                                        TempHigh = 18;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                    case '9':
                                        TempHigh = 19;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET TempHigh  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" TempHigh = ");
                                           lcd.print(TempHigh);
                                            break;
                                  }
                    }
               }
        }
}
void Controller(){
  //MANUAL
                      //Serial.println(key);   
                      password.append(key); 
                      passinput = 1;  
                      if(key == '*'){  
                        password.reset();     
                        passinput = 0;  
                        locked = 1;
                      }
                      if(password.evaluate()) {       
                        locked = !locked; 
                        password.reset();            
                        passinput = 0;
                      }
                      if(!locked){
                         while(key != '9'){
                          key = keypad.getKey();
                          password.append(key);
                          passinput = 1;  
                          lcd.setCursor(0,0);
                          lcd.print(" CONTROL PROGRAM ");  
                          if(key== '1' || key=='2' || key=='3' || key=='4'|| key=='7' || key=='8'){
                                            lcd.setCursor(0,0);
                                            lcd.print("CONTROL PROGRAM");
                                            lcd.setCursor(0,1);
                                            lcd.print(" PLEASE  SELECT ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" A: FAN , 9 OUT ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Water_Pump: B  ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print("   Cooler:  C   ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Cooler_Pump: D  ");
                                            delay(1500);
                                            lcd.setCursor(0,1);
                                            lcd.print(" SET_Humi: 6  ");
                                            delay(1500);    
                                            lcd.setCursor(0,1);
                                            lcd.print(" SET_Temp: 5  ");
                                            delay(1500);    
                          }
                          if(key == '5'){
                               SET_TEMP();
                          }
                          if(key == '6'){
                               SET_HUMI();
                          }
                             switch (key) {             
                                case 'A':
                                      digitalWrite(FAN_IN_OUT,  !digitalRead(FAN_IN_OUT));
                                        if(digitalRead(FAN_IN_OUT)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("FAN: OFF");
                                        }
                                        if(digitalRead(FAN_IN_OUT)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("FAN: OPEN");
                                        }
                                      break;
                                case 'B':
                                      digitalWrite(WATER_PUMP, !digitalRead(WATER_PUMP));
                                      if(digitalRead(WATER_PUMP)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("WATER_PUMP: OFF");
                                        }
                                        if(digitalRead(WATER_PUMP)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("WATER_PUMP:OPEN");
                                        }
                                      break;
                                case 'C':
                                      digitalWrite(COOLER, !digitalRead(COOLER));
                                      if(digitalRead(COOLER)==HIGH){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("SYSTEM_COOL:OFF");
                                        }
                                        if(digitalRead(COOLER)==LOW){
                                          lcd.clear();
                                          lcd.setCursor(0,1);
                                          lcd.print("SYSTEM_COOL:OPEN");
                                        }
                                      break;
                                case 'D':
                                      digitalWrite(COOLER_PUMP, !digitalRead(COOLER_PUMP));
                                      if(digitalRead(COOLER_PUMP)==HIGH){
                                              lcd.clear();
                                              lcd.setCursor(0,1);
                                              lcd.print("COOLER_PUMP:OFF");
                                        }
                                      if(digitalRead(COOLER_PUMP)==LOW){
                                              lcd.clear();
                                              lcd.setCursor(0,1);
                                              lcd.print("COOLER_PUMP:OPEN");
                                         }
                                       break; 
                                 case '#':
                                            lcd.setCursor(0,0);
                                            lcd.print("CONTROL PROGRAM");
                                            lcd.setCursor(0,1);
                                            lcd.print(" A: FAN , 9 OUT ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Water_Pump: B  ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print("   Cooler:  C   ");
                                            delay(1000);
                                            lcd.setCursor(0,1);
                                            lcd.print(" Cooler_Pump: D  ");
                                            delay(1000);
                                       break;
                        }
                            delay(100);
                      }
                      password.reset();
                   }
}
void SET_HUMI(){
        key = keypad.getKey();
        //Serial.println(key); 
        lcd.clear(); 
        lcd.setCursor(0,0);
        lcd.print("  SET Humidity  ");
        lcd.setCursor(0,1);
        lcd.print(" Humidity = ");
        while(key != '*'){
              //Serial.print("ENTER 6");
              key = keypad.getKey();
              //Serial.println(key);
                  if(key=='5'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 5");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET Humidity  ");;
                    lcd.setCursor(0,1);
                    lcd.print(" Humidity = ");
                    lcd.print("5");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                         HumiHigh = 50;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '1':
                                            HumiHigh = 51;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '2':
                                         HumiHigh = 52;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '3':
                                         HumiHigh = 53;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '4':
                                            HumiHigh = 54;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '5':
                                            HumiHigh = 55;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '6':
                                         HumiHigh = 56;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '7':
                                            HumiHigh = 57;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '8':
                                         HumiHigh = 58;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '9':
                                            HumiHigh = 59;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                    
                                  }
                    }
               }
                  if(key=='6'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 6");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET Humidity  ");;
                    lcd.setCursor(0,1);
                    lcd.print(" Humidity = ");
                    lcd.print("6");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                         HumiHigh = 60;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '1':
                                            HumiHigh = 61;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '2':
                                         HumiHigh = 62;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '3':
                                         HumiHigh = 63;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '4':
                                            HumiHigh = 94;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '5':
                                            HumiHigh = 65;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '6':
                                         HumiHigh = 66;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '7':
                                            HumiHigh = 67;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '8':
                                         HumiHigh = 68;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '9':
                                            HumiHigh = 69;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                    
                                  }
                    }
               }
                 if(key=='7'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 7");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET Humidity  ");;
                    lcd.setCursor(0,1);
                    lcd.print(" Humidity = ");
                    lcd.print("7");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                    case '0':
                                         HumiHigh = 70;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '1':
                                            HumiHigh = 71;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '2':
                                         HumiHigh = 72;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '3':
                                         HumiHigh = 73;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '4':
                                            HumiHigh = 74;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '5':
                                            HumiHigh = 75;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '6':
                                         HumiHigh = 76;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '7':
                                            HumiHigh = 77;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '8':
                                         HumiHigh = 78;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '9':
                                            HumiHigh = 79;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                    
                                  }
                    }
               }
                if(key=='8'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 8");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET Humidity  ");;
                    lcd.setCursor(0,1);
                    lcd.print(" Humidity = ");
                    lcd.print("8");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                         HumiHigh = 80;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '1':
                                            HumiHigh = 81;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '2':
                                         HumiHigh = 82;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '3':
                                         HumiHigh = 83;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '4':
                                            HumiHigh = 94;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '5':
                                            HumiHigh = 85;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '6':
                                         HumiHigh = 86;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '7':
                                            HumiHigh = 87;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '8':
                                         HumiHigh = 88;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '9':
                                            HumiHigh = 89;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                    
                                  }
                    }
               }
                if(key=='9'){
                    key = keypad.getKey();
                    //Serial.print("ENTER 9");
                    lcd.clear();
                    lcd.setCursor(0,0);
                    lcd.print("  SET Humidity  ");;
                    lcd.setCursor(0,1);
                    lcd.print(" Humidity = ");
                    lcd.print("9");
                    while(key !='*'){
                            key = keypad.getKey();
                              //Serial.println(key);
                                  switch(key){
                                     case '0':
                                         HumiHigh = 90;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '1':
                                            HumiHigh = 91;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '2':
                                         HumiHigh = 92;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '3':
                                         HumiHigh = 93;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '4':
                                            HumiHigh = 94;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '5':
                                            HumiHigh = 95;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '6':
                                         HumiHigh = 96;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '7':
                                            HumiHigh = 97;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '8':
                                         HumiHigh = 98;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                     case '9':
                                            HumiHigh = 99;
                                           lcd.clear();
                                           lcd.setCursor(0,0);
                                           lcd.print("  SET Humidity  ");
                                           lcd.setCursor(0,1);
                                           lcd.print(" Humidity = ");
                                           lcd.print(HumiHigh);
                                            break;
                                    
                                  }
                    }
               }
        }
}
