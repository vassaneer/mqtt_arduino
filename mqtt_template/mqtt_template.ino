#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include <stdlib.h>

// Update these with values suitable for your network.
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Config MQTT Server
#define mqtt_server "IP ADDRESS SERVER"
#define mqtt_port 1883
#define mqtt_user "TEST"
#define mqtt_password ""

#define LED_PIN 2

// declare device from id
const char* DEVICE = "1";
const char* DHT22 = "1";
const char* PZEM = "2";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(25200);
  
  client.setServer(mqtt_server, mqtt_port);
  timeClient.begin();
//  client.setCallback(callback);
}
void loop() {
  if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      timeClient.update();
      unsigned long epochTime = timeClient.getEpochTime();
      char epoch [sizeof(unsigned long)*8+1];
      ultoa (epochTime,epoch,10);
      char str[100];
      /*
        send to mqtt broker
        - timestamp
        - device_id
        - sensor
        - value
      */
      strcpy (str,"timestamp=");
      strcat (str,epoch);
      strcat (str,",device=");
      strcat (str,DEVICE);
      strcat (str,",sensor=");
      strcat (str,DHT22);
      strcat (str,",value=");
      char value[10];
      // value from sensor DHT22, PZEM
      snprintf(value, sizeof(value), "%.2f", 2.31);
      strcat (str,value);
      client.publish("/ESP/LED",str);
  }
  delay(2000);
}
