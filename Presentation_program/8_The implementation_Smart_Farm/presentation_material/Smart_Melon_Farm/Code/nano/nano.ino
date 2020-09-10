//#include <Arduino.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#define LDRENVI1 A0
#define LDRENVI2 A1
#define MOISTURE1 A2
#define MOISTURE2 A3
#define MOISTURE3 A6
#define PUMP 4

/*
YF‐ S201 Water Flow Sensor
Water Flow Sensor output processed to read in litres/hour
Adaptation Courtesy: www.hobbytronics.co.uk
*/
volatile int flow_frequency; // Measures flow sensor pulses
// unsigned int l_hour;         // Calculated litres/hour
unsigned int l_min;
unsigned char flowsensor = 2; // Sensor Input
unsigned long cloopTime;

// define keypad row and column
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
    {'1', '4', '7', '*'},
    {'2', '5', '8', '0'},
    {'3', '6', '9', '#'},
    {'A', 'B', 'C', 'D'}};
byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8, 7, 6, 5};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
// set the LCD address to 0x27 for a 16 chars and 2 line display
// 0x3F for nano
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Serial pin that sent to esp8266
SoftwareSerial espSerial(3, 4); // TX, RX

float ldrs[] = {LDRENVI1, LDRENVI2};
float moistures[] = {MOISTURE1, MOISTURE2, MOISTURE3};

// initialize LDR sensors array to store read() values
float ldr[2];
float moisture[3];

char modecode = 'A';          // mode input
char password[8];             // password input
char standardcode[6] = "50";  // soil moisture standard number
String cridential = "202001"; // credentilal privacy, set your password here

// Timers auxiliar variables
unsigned long now = millis();
unsigned long lastMeasure = 0;

void flow() // Interrupt function
{
  flow_frequency++;
}

void setup()
{
  pinMode(PUMP,OUTPUT);
  digitalWrite(PUMP, HIGH);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);   // Optional Internal Pull-Up
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei();                            // Enable interrupts
  now = millis();
  cloopTime = now;

  Serial.begin(115200);
  while (!Serial)
    continue;
  espSerial.begin(4800);
  // initialize the lcd.
  lcd.begin();
  lcd.backlight();
  // input setup of light sensors
  for (int index = 0; index < 2; index++)
  {
    pinMode(ldrs[index], INPUT);
  }
  // input setup of soil moisture sensors
  for (int index = 0; index < 3; index++)
  {
    pinMode(moistures[index], INPUT);
  }
  delay(2000);
}

void meter()
{
  now = millis();
  // Every second, calculate and print litres/hour
  if (now >= (cloopTime + 1000))
  {
    cloopTime = now; // Updates cloopTime
    // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
    // l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
    l_min = (flow_frequency / 7.5);
    flow_frequency = 0; // Reset Counter
  }
  // Serial.print(l_hour, DEC); // Print litres/hour
  // Serial.println(" L/hour");
  // Serial.print(l_min, DEC); // Print litres/hour
  // Serial.print(l_min);
  // Serial.println(" L/minute");
}

void ldrsEnvi()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  for (int index = 0; index < 2; index++)
  {
    ldr[index] = analogRead(ldrs[index]);
    ldr[index] = map(ldr[index], 0, 1023, 0, 100);
  }
}

void moisturesEnvi()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  for (int index = 0; index < 3; index++)
  {
    moisture[index] = analogRead(moistures[index]);
    moisture[index] = map(moisture[index], 0, 1023, 100, 0);
  }
}

void getkey()
{
  char key = keypad.getKey();
  if (key)
  {
    Serial.println(key);
  }

  // when the user presses 'C' on the keypad, him/ her will guide to the login mode
  if (key == 'C')
  {
    lcd.clear();
    key = keypad.getKey();
    lcd.setCursor(5, 0);
    lcd.print("Login");
    lcd.setCursor(0, 1);
    lcd.print("Password:");
    memset(password, 0, sizeof(password)); // clear lastest password
    do
    {
      key = keypad.getKey();
      // numbers are allowed to set for password only
      if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
      {
        // password length is required only 6 number
        if (strlen(password) < 6)
        {
          // extent input from keypad to password valiable
          int num = +strlen(password);
          password[num] = key;
          delay(100);
        }
        // print start instead of password as hiden
        switch (strlen(password) + 8)
        {
        case 9:
          lcd.setCursor(9, 1);
          lcd.print('*');
          break;
        case 10:
          lcd.setCursor(10, 1);
          lcd.print('*');
          break;
        case 11:
          lcd.setCursor(11, 1);
          lcd.print('*');
          break;
        case 12:
          lcd.setCursor(12, 1);
          lcd.print('*');
          break;
        case 13:
          lcd.setCursor(13, 1);
          lcd.print('*');
          break;
        case 14:
          lcd.setCursor(14, 1);
          lcd.print('*');
          break;

        default:
          break;
        }
      }
    } while (key != 'B');
    lcd.clear();
  }

  // convert char to string
  String passwordStr(password);
  // login privacy
  if (passwordStr == cridential)
  {
    lcd.clear();
    do
    {
      // select the system mode
      key = keypad.getKey();
      lcd.setCursor(0, 0);
      lcd.print("auto mode: A");
      lcd.setCursor(0, 1);
      lcd.print("manual mode: D");
      lcd.setCursor(0, 2);
      lcd.print("set moisture: C");
      lcd.setCursor(0, 3);
      lcd.print("on-off pump: 1");
      if (key == 'A') // set to automatic mode
      {
        lcd.clear();
        do
        {
          modecode = 'A';
          key = keypad.getKey();
          lcd.setCursor(3, 0);
          lcd.print("smart farm");
          lcd.setCursor(0, 1);
          lcd.print("on auto mode");
        } while (key != 'B');
      }
      else if (key == 'D') // set to manual mode
      {
        lcd.clear();
        do
        {
          modecode = 'M';
          key = keypad.getKey();
          lcd.setCursor(3, 0);
          lcd.print("smart farm");
          lcd.setCursor(0, 1);
          lcd.print("on manual mode");
        } while (key != 'B');
      }
      else if (key == 'C') // set soil moisture standard number
      {
        lcd.clear();
        do
        {
          key = keypad.getKey();
          lcd.setCursor(3, 0);
          lcd.print("smart farm");
          lcd.setCursor(0, 1);
          lcd.print("soil number:");
          lcd.setCursor(12, 1);
          lcd.print(standardcode);

          if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
          {
            // standardcode length is required only 2 number
            if (strlen(standardcode) < 2)
            {
              // extent input from keypad to standardcode valiable
              int num = +strlen(standardcode);
              standardcode[num] = key;
              delay(100);
            }
          }
          else if (key == '*')    
          {
            memset(standardcode, 0, strlen(standardcode));
            lcd.clear();
          }
          
        } while (key != 'B');
        
      }else if(key == '1')
          {
            do{
            if(key == '1'){
              digitalWrite(PUMP, LOW);
              lcd.clear();
              lcd.setCursor(2, 0);
              lcd.print("smart melon farm");
              lcd.setCursor(5, 1);
              lcd.print("pump is on");
              lcd.setCursor(0, 3);
              lcd.print("press 2 for turn off");
            }
            if(key == '2'){
              digitalWrite(PUMP, HIGH);
              lcd.clear();
              lcd.setCursor(2, 0);
              lcd.print("smart melon farm");
              lcd.setCursor(5, 1);
              lcd.print("pump is off");
              lcd.setCursor(0, 3);
              lcd.print("press 1 for turn on");
            }
            key = keypad.getKey();
            }while(key != 'B');
              digitalWrite(PUMP, HIGH);
            
          }
    } while (key != 'B');
    digitalWrite(PUMP, HIGH);
    
    memset(password, 0, sizeof(password)); // clear lastest password
    lcd.clear();
  }
}

void transfer()
{
  StaticJsonDocument<200> doc;
  doc["light1"] = ldr[0];
  doc["light2"] = ldr[1];

  doc["moisture1"] = moisture[0];
  doc["moisture2"] = moisture[1];
  doc["moisture3"] = moisture[2];

  doc["meter"] = l_min;

  doc["modecode"] = modecode;
  doc["standardcode"] = standardcode;

  serializeJson(doc, espSerial);
}

void monitor()
{
  lcd.setCursor(11, 1);
  lcd.print("in_l:");
  lcd.print((int)ldr[0]);
  lcd.setCursor(11, 2);
  lcd.print("out_l:");
  lcd.print((int)ldr[1]);

  lcd.setCursor(0, 1);
  lcd.print("sm1:");
  lcd.print((int)moisture[0]);
  lcd.setCursor(0, 2);
  lcd.print("sm2:");
  lcd.print((int)moisture[1]);
  lcd.setCursor(0, 3);
  lcd.print("sm3:");
  lcd.print((int)moisture[0]);

  lcd.setCursor(11, 3);
  lcd.print("mt:");
  lcd.print(l_min, DEC);
  lcd.setCursor(16, 3);
  lcd.print("l/m");
}

void loop()
{
  meter();
  getkey();
  ldrsEnvi();
  moisturesEnvi();

  now = millis();
  if (now - lastMeasure > 10000)
  {
    lastMeasure = now;
    transfer();
  }

  if (now - lastMeasure > 5000)
  {
    lastMeasure = now;
    monitor();
  }

  lcd.setCursor(2, 0);
  lcd.print("smart melon farm");



  // Serial.println(ldr[0]);
  // Serial.println(ldr[1]);
}
