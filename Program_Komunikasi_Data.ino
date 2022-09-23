#include <ESP8266WiFi.h> //library for wifi esp8266
#include <PubSubClient.h> //library mqtt
#include <HCSR04.h> //library proximity sensor 
#include <EEPROM.h> //library eeprom for memory

const char *ssid = "PKL";
const char *password = "12345678";
const char *MQTT_SERVER = "broker.mqtt-dashboard.com";
const char *RELAY = "esp/input/jarak";
#define MQTT_JARAK "esp/pkl/jarak"
#define MQTT_DURASI "esp/pkl/durasi"
#define relayPin D0
#define triggerPin D1
#define echoPin D2
long duration; 
int jarak1;
unsigned long startMillis=0;
int a,b,c;
WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void jarak_sensor(){
  long duration;
  int jarak1;
  unsigned long currentMillis = millis();// variabel mengambil waktu yang berjalan dan menyimpan di variabel current millis
  if (currentMillis - startMillis >= 2000) { //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 2000
    startMillis = currentMillis;
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2); 
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(triggerPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    jarak1 = (duration/2) / 29.1;
    
    Serial.print("Jarak :");
    Serial.print(jarak1);
    Serial.println(" cm");
    client.publish(MQTT_JARAK, String(jarak1).c_str());
    client.publish(MQTT_DURASI, String(duration).c_str());
    
    if (jarak1 >= EEPROM.read(0)){
        digitalWrite(relayPin, HIGH);
        Serial.println("Relay Mati");
      }
     else if (jarak1 < EEPROM.read(0)){
        digitalWrite(relayPin, LOW);
        Serial.println("Relay Hidup");
      }
      else {
        Serial.println("");
      }
    }     
  }

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("espClient")) 
    {
      Serial.println("connected");
      client.subscribe("esp/input/jarak");
      client.subscribe("esp/input/2jarak");
      client.subscribe("esp/input/3jarak");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
 pinMode(relayPin, OUTPUT);
 pinMode(triggerPin, OUTPUT);
 pinMode(echoPin, INPUT);
 setup_wifi();
 Serial.begin(115200);
 EEPROM.begin(512);
 client.setServer(MQTT_SERVER, 1883);
 client.setCallback(callback);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 Serial.print("MAC Address: ");
 Serial.println(WiFi.macAddress());
 WiFi.setAutoReconnect(true);
 WiFi.persistent(true);

 EEPROM.read(0);
 Serial.print("Nilai Awal A : ");
 Serial.println(EEPROM.read(0));
 EEPROM.read(5);
 Serial.print("Nilai Awal B : ");
 Serial.println(EEPROM.read(5));
 EEPROM.read(9);
 Serial.print("Nilai Awal C : ");
 Serial.println(EEPROM.read(9));
 delay(1000);

 }

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String payload;
  
  for (int i = 0; i < length; i++)
  {
    
    Serial.print((char)message[i]);
    Serial.println();
    if ((char)message[i] != '"')
      payload += (char)message[i];
   }
    if (String(topic)=="esp/input/jarak") {
        a = payload.toInt();
        Serial.print("Nilai A = ");
      Serial.println(a);
      EEPROM.write(0, a);
      EEPROM.commit();
      }
    if (String(topic)=="esp/input/2jarak") {
        b = payload.toInt();
        Serial.print("Nilai B = ");
      Serial.println(b);
      EEPROM.write(5, b);
      EEPROM.commit();
      }
   if (String(topic)=="esp/input/3jarak") {
        c = payload.toInt();
        Serial.print("Nilai C = ");
        Serial.println(c);
         EEPROM.write(9, c);
         EEPROM.commit();
      }
  }
 void loop() {
 jarak_sensor();
     if (!client.connected()){
      reconnect();
    }
     client.loop();
 }
