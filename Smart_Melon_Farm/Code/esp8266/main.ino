#include <Arduino.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN1 5 // D1
#define DHTPIN2 4 // D2

// Uncomment whatever type you're using!
// #define DHTTYPE DHT11 // DHT 11
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

// Change the credentials below, so your ESP8266 connects to your router
// const char *ssid = "CEIT-IoT";
// const char *password = "IoT12345678";
// const char *ssid = "CEIT-SOFTWARE";
// const char *password = "ceitSoftw@re2020";
const char *ssid = "Smart IoT";
const char *password = "iot@2020";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char *mqtt_server = "192.168.9.75";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

// Initialize DHT sensor. change the line below whatever DHT type you're using DHT11, DHT21 (AM2301), DHT22 (AM2302, AM2321)
DHT dht[] = {{DHTPIN1, DHTTYPE}, {DHTPIN2, DHTTYPE}};
// initialize temperatures and humidities array to store read() values
float humids[2];
float temps[2];

// Timers auxiliar variables
unsigned long now = millis();
unsigned long lastMeasure = 0;

// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("envsensors"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(4800);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  while (!Serial)
    continue;
  // DHTs setup
  for (auto &sensor : dht)
  {
    sensor.begin();
  }

  delay(2000);
}

void dhtsEnvi()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  for (int index = 0; index < 2; index++)
  {
    humids[index] = dht[index].readHumidity();
    temps[index] = dht[index].readTemperature();

    String humid1 = (String)humids[0];
    String humid2 = (String)humids[1];
    String temp1 = (String)temps[0];
    String temp2 = (String)temps[1];
    char msghumid1[16];
    char msghumid2[16];
    char msgtemp1[16];
    char msgtemp2[16];
    humid1.toCharArray(msghumid1, humid1.length());
    humid2.toCharArray(msghumid2, humid2.length());
    temp1.toCharArray(msgtemp1, temp1.length());
    temp2.toCharArray(msgtemp2, temp2.length());

    now = millis();
    if (now - lastMeasure > 10000)
    {
      lastMeasure = now;
      client.publish("local/humid1", msghumid1);
      client.publish("local/humid2", msghumid2);
      client.publish("local/temp1", msgtemp1);
      client.publish("local/temp2", msgtemp2);
      delay(100);
    }
  }
}

void receiver()
{
  if (Serial.available())
  {
    // Create new documents must be bigger than sender because it must store the strings
    StaticJsonDocument<300> doc;
    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial);

    if (err == DeserializationError::Ok)
    {
      // Convert float to string
      // dtostrf(floatvar, StringLengthIncDecimalPoint, numVarsAfterDecimal, charbuf);
      // floatvar  float variable
      // StringLengthIncDecimalPoint  This is the length of the string that will be created
      // numVarsAfterDecimal  The number of digits after the deimal point to print
      // charbuf  the array to store the results
      static char light1[8];
      static char light2[8];
      static char moisture1[8];
      static char moisture2[8];
      static char moisture3[8];
      static char meter[8];
      static char modecode[4];
      static char standardcode[4];

      dtostrf(doc["light1"].as<float>(), 4, 2, light1);
      dtostrf(doc["light2"].as<float>(), 4, 2, light2);
      dtostrf(doc["moisture1"].as<float>(), 4, 2, moisture1);
      dtostrf(doc["moisture2"].as<float>(), 4, 2, moisture2);
      dtostrf(doc["moisture3"].as<float>(), 4, 2, moisture3);
      dtostrf(doc["meter"].as<long int>(), 2, 0, meter);
      dtostrf(doc["modecode"].as<char>(), 1, 0, modecode);
      dtostrf(doc["standardcode"].as<char>(), 2, 0, standardcode);

      client.publish("local/light1", light1);
      client.publish("local/light2", light2);
      client.publish("local/moisture1", moisture1);
      client.publish("local/moisture2", moisture2);
      client.publish("local/moisture3", moisture3);
      client.publish("local/meter", meter);

      client.publish("local/modecode", modecode);
      client.publish("local/standardcode", standardcode);
      //   delay(100);

      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      Serial.print("inside light = ");
      Serial.print(doc["light1"].as<float>());
      Serial.print("  outside light = ");
      Serial.println(doc["light2"].as<float>());

      Serial.print("moisture 1 = ");
      Serial.print(doc["moisture1"].as<float>());
      Serial.print("  moisture 2 = ");
      Serial.print(doc["moisture2"].as<float>());
      Serial.print("  moisture 3 = ");
      Serial.println(doc["moisture3"].as<float>());

      Serial.print("meter: ");
      Serial.println(doc["meter"].as<long int>());

      Serial.print("mode code = ");
      Serial.print(doc["modecode"].as<char>());
      Serial.print("  standard code= ");
      Serial.println(doc["standardcode"].as<String>());
    }
    else
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial.available() > 0)
        Serial.read();
    }
  }
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect("envsensors");
  }

  //reconncet the mqtt broker if there is lost connection
  // if (!client.connected())
  // {
  //   reconnect();
  // }
  // client.loop();

  dhtsEnvi();
  receiver();
}
