#include <WiFi.h>
#include <PubSubClient.h>

// Wifi Setting
// For testing at home change to you wifi name and password
// #define WIFI_STA_NAME "UTCC-Maker Space"  // Wifi Name at Utcc AI+IOT
// #define WIFI_STA_PASS "Makerspace"        // Wifi Passwork at Utcc AI+IOT

 #define WIFI_STA_NAME "wjeab2016"  // Wifi Name at Utcc AI+IOT
 #define WIFI_STA_PASS "@wjeab2016"        // Wifi Passwork at Utcc AI+IOT



// Mqtt Broker Setting
// #define MQTT_SERVER   "Homeassistant broker IP" 
#define MQTT_SERVER "192.168.1.86"   // Change to your Homeassistant IP  
#define MQTT_PORT     1883
#define MQTT_USERNAME "user"   // Hone assistantuser or Mqtt user
#define MQTT_PASSWORD "@user"  // Homeassistan user passwork or Mqtt user password
#define MQTT_NAME     ""

// Set up command topic for receicing command to control device
// Set up Sate topic for send back device state back to Homeassistant after done command
#define command_topic_Led1 "home/tm00/esp32/command/led/1"
#define state_topic_Led1 "home/tm00/esp32/state/led/1"
#define payload_on_Led "on"
#define payload_off_Led "off"
 

#define LED_PIN 2  // On Board Led of Esp32

WiFiClient client;
PubSubClient mqtt(client);
boolean isTurnOn = true;

//-----------------------------------------------------
// Get Payload from topic that esp32 was scribed at setup function
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String topic_str = topic, payload_str = (char*)payload;
  Serial.print(WiFi.localIP());
  Serial.println(" [" + topic_str + "]: " + payload_str);
   
  if (payload_str == "on") { 
       digitalWrite(LED_PIN, HIGH ); 
       if (mqtt.connected() == true) {   mqtt.publish(state_topic_Led1, payload_on_Led);  }
       Serial.print("-- LED ON , public : ");  Serial.println(payload_on_Led);

  } else if (payload_str == "off") { 
       digitalWrite(LED_PIN, LOW); 
       if (mqtt.connected() == true) {  mqtt.publish(state_topic_Led1, payload_off_Led); }
       Serial.print("-- LED OFF , public : ");  Serial.println(payload_off_Led);

  } else
  {
      Serial.println("-- No Match Command !!!");
  }
 
}
  
//-----------------------------------------------------
void setup() {
  Serial.begin(115200);
//  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
 
 
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");  Serial.println(WIFI_STA_NAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  digitalWrite(LED_PIN, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}
//-----------------------------------------------------
void loop() {
  if (mqtt.connected() == false) {
    Serial.print("MQTT connection... ");
    if (mqtt.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
       
          Serial.println("connected");
          mqtt.subscribe(command_topic_Led1);
          Serial.print("Topic   : ");  Serial.println(command_topic_Led1);
          Serial.println("Command : ON,OFF");

          if (isTurnOn) { 

              mqtt.publish(state_topic_Led1, payload_on_Led); isTurnOn = false;
          }

    } else {
      Serial.println("failed");
      delay(5000);
    }
  } else {

    mqtt.loop();

    
  }
}
