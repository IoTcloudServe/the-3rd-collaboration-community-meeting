#include <ESP8266WiFi.h>
#include <MicroGear.h>

const char* ssid     = "Straw@IoT";
const char* password = "iot@ceit";

#define APPID   "LaoSmartfarm"  
#define KEY     "4F8dSA9hfL0cjYb" 
#define SECRET  "OPH3LsTf5Wi5T52wEll5kbB7d" 
#define ALIAS   "Soilsensor"
#define FEEDID  "soiData"

int sense_Pin= A0; // Soil Sensor input at Analog PIN A0
int value= 0;

WiFiClient client;

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
  microgear.publish("/soil",msgstr);
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


void setup() {
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");
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

      /* Call this method regularly otherwise the connection may be lost */
      microgear.loop();
      Serial.print("MOISTURE LEVEL : ");
      value= analogRead(sense_Pin);
      Serial.println(value);
      msgstr = (String)value;
      Serial.println(msgstr);
          Serial.println("Publish...");
          microgear.publish("/soil",msgstr);
          String data = "{\"soil\":"; 
                          data += value; 
                          data += "}";  
                          microgear.writeFeed(FEEDID,data);
      timer = 0;
    }
    else {
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
