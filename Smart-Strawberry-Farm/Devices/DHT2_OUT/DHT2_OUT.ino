#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "DHT.h"

const char* ssid     = "Straw@IoT";
const char* password = "iot@ceit";

#define APPID   "LaoSmartfarm"  
#define KEY     "4F8dSA9hfL0cjYb" 
#define SECRET  "OPH3LsTf5Wi5T52wEll5kbB7d" 
#define ALIAS   "dht2"
#define FEEDID  "DHTOut"

#define DHTPIN D4
#define DHTTYPE DHT22
#define REDPIN D7
#define GREENPIN D6
#define BLUEPIN D5
#define LEDPIN D3

WiFiClient client;

DHT dht(DHTPIN, DHTTYPE);
int LDR = A0;
int humid = 0;
int temp = 0;
int light = 0;
int counter = 0;
String msgstr;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();  
  microgear.publish("/dht2",msgstr);
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void WiFiEvent(WiFiEvent_t event) {
    Serial.printf("[WiFi-event] event: %d\n", event);

    switch(event) {
        case WIFI_EVENT_STAMODE_GOT_IP:
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(WiFi.localIP());
            digitalWrite(BLUEPIN, LOW);
            break;
        case WIFI_EVENT_STAMODE_DISCONNECTED:
            Serial.println("WiFi lost connection");
            digitalWrite(BLUEPIN, HIGH);
            break;
    }
}

void setup() {
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    dht.begin();
    pinMode(REDPIN, OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN, OUTPUT);
    pinMode(LEDPIN, OUTPUT);
    digitalWrite(REDPIN, LOW);
    digitalWrite(GREENPIN, LOW);
    digitalWrite(BLUEPIN, LOW);
    digitalWrite(LEDPIN, LOW);

    if(WiFi.begin(ssid,password)){
      while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
      }
    }
    Serial.println("WIFI CONNECTED");
    Serial.println("IP ADRESS");
    Serial.println(WiFi.localIP());
    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(KEY,SECRET,ALIAS);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
     /* To check if the microgear is still connected */
   if (microgear.connected()) {
      Serial.println("connected");
      digitalWrite(GREENPIN, HIGH);


      /* Call this method regularly otherwise the connection may be lost */
      microgear.loop();

      float LDRValue = (float)analogRead(LDR); 
      if(LDRValue<0){
        LDRValue = 0.0;
      }else if(LDRValue>600){
//        LDRValue = 600.0;
      }
      LDRValue = LDRValue;
      Serial.println(LDRValue);
      if(LDRValue<=3){
        digitalWrite(LEDPIN, HIGH);
      }else{
        digitalWrite(LEDPIN, LOW);
      }
      
      float vhud = dht.readHumidity();
      float vtmp = dht.readTemperature();
      if (isnan(vhud) || isnan(vtmp) || vhud > 100 || vtmp > 100){
        vhud = 0.0;
        vtmp = 0.0;
      }
      LDRValue = 640;
      msgstr = (String)vtmp+","+(String)vhud+","+LDRValue+",esp8266";
      Serial.println(msgstr);
      Serial.println(humid!=(int)vhud || temp!=(int)vtmp || light!=(int)LDRValue);

      if(humid!=(int)vhud || temp!=(int)vtmp || light!=(int)LDRValue){
        Serial.println("Publish...");
        microgear.publish("/dht2",msgstr);
        humid=(int)vhud;
        temp=(int)vtmp;
        light=(int)LDRValue;
        counter=0;  
      }else{
        if(counter==5){
          Serial.println("Publish...");
          microgear.publish("/dht2",msgstr);
          String data = "{\"Humid\":"; 
                          data += vhud; 
                          data += ", \"Temp\":"; 
                          data += vtmp;
                          data += ", \"Light\":"; 
                          data += LDRValue ; 
                          data += "}";  
                          microgear.writeFeed(FEEDID,data);
          counter=0;  
        } 
        counter++;
      }
      timer = 0;
    }
    else {
      digitalWrite(GREENPIN, HIGH);
      if (timer >= 5000) {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID);
        timer = 0;
      }
      else timer += 500;
    }
    delay(500);
  }
}
