
/*
 * Asep Trisna Setiawan
 */
//library sensor suhu
#include <Wire.h>
#include <Adafruit_MLX90614.h>  
// Library wifi
#include <EasyNTPClient.h>
#include <FS.h> 
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
//Libray Jsonfile
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "config.h"
char payloadTemp[200];
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float TempReading;
int soil_sensor= A0;
int powerPin = 15;
int Status;

WiFiUDP udp;
EasyNTPClient ntpClient(udp, "pool.ntp.org", 25200); 
StaticJsonBuffer<200> jsonBuffer;
//var untuk userID
String commandMessage1 = "1"; //message pesan subscribe
String commandMessage2 = "2"; //message pesan publish
String typeDevice = "Soil Sensor";
char routingkey1[40] = "deteksimasker";
int led1 = D3 ;
int led2 = D4 ;
int buzzer = D6 ;
void setup_wifi() {
  WiFi.macAddress(MAC_array);
  for (int i = 0; i < sizeof(MAC_array) - 1; ++i) {
    sprintf(MAC_char, "%s%02x:", MAC_char, MAC_array[i]);
  }
  sprintf(MAC_char, "%s%02x", MAC_char, MAC_array[sizeof(MAC_array) - 1]);
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  String(mqtt_port).toCharArray(smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", smqtt_port, 5);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 40);
  WiFiManagerParameter custom_mqtt_password("password", "mqtt password", mqtt_password, 40);
  WiFiManagerParameter custom_mqtt_keywords1("keyword1", "mqtt keyword1", MAC_char, 40);
  //WiFiManagerParameter custom_mqtt_keywords2("keyword2", "mqtt keyword2", mqtt_keywords2, 40);
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter( & custom_mqtt_server);
  wifiManager.addParameter( & custom_mqtt_port);
  wifiManager.addParameter( & custom_mqtt_user);
  wifiManager.addParameter( & custom_mqtt_password);
  wifiManager.addParameter(&custom_mqtt_keywords1);
  // wifiManager.addParameter( & custom_mqtt_keywords2);
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoCon nectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect(MAC_char, "password")) {
    Serial.println("failed to connect and hit timeout");
    delay(2000);
    //reset and try again, or maybe put it to deep sleep
//    ESP.reset();
    delay(2000);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//calback
void callback(char * topic, byte * payload, unsigned int length) {
  char message [7] ;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  for (int i = 0; i < length; i++) {
    message[i] = (char)payload[i];
  }
  String convertMsg = String(message) ;
  String data = convertMsg.substring(5);
  Serial.println (data);
  int timer = data.toInt();

  //relay 1 (Koding led)
  if (message[0] == '0') {
    Serial.println("SELAMAT DATANG");
     digitalWrite(led2, HIGH);
     sensor();
     delay(100);
     digitalWrite(led2, LOW); 
    
  } else if (message[0] == '1') {
    Serial.println("ANDA TIDAK MENGGUNKAN MASKER");
    digitalWrite(led1, HIGH); 
    digitalWrite(buzzer, HIGH);
    sensor();
    delay(100);
     digitalWrite(buzzer, LOW );
     digitalWrite(led1, LOW);       
  }
 }
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...%s");
    Serial.println(mqtt_server);
    // Attempt to connect
    if (client.connect(MAC_char, mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println(MAC_char);
      client.subscribe(MAC_char);
//      client.subscribe(MAC_char+1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      if (client.state() == 4) ESP.restart();
      else {
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        //delay(10000);
      }
    }
  }
}

void setup() {
  //setup pin mode
  //  pinMode(soilSensor, INPUT_PULLUP);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D6, OUTPUT);
  mlx.begin();
  Serial.begin(9600);
  Serial.println(F("Booting...."));
  //read config wifi,mqtt dan yang lain
  ReadConfigFile();
  setup_wifi();
  SaveConfigFile();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  TempReading = mlx.readObjectTempC();
//  Serial.println(TempReading);
//  delay(1000);
//  digitalWrite(led1, LOW);
//  digitalWrite(led2, LOW);  
//  sensor();
  
}

void sensor(){
//  Koding Sensor Suhu
   TempReading = mlx.readObjectTempC();
   Serial.println(TempReading);
  if (TempReading > 37){
      Serial.println("MAAF SUHU ANDA TINGGI");
//      digitalWrite(led2, HIGH); 
//      digitalWrite(buzzer, HIGH);
//      delay(1000);
//      digitalWrite(buzzer, LOW )  ; 
    }else if(TempReading < 37){ 
     Serial.println("SELAMAT DATANG");
//     digitalWrite(led1, HIGH);
//     delay(1000);
    }else{
      Serial.println("Suhu Tidak Terbaca");
    }


   
  float unixtime = ntpClient.getUnixTime();
  Serial.println(unixtime);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["mac"] = MAC_char;
  root["suhu"] =TempReading+2;
//  root["ampere"] =amper;
//  root["watt"] =watt;
//  root["kwh"] =kwh;
//  root["Status"] =Status;
////  root["sn"] = MAC_char;
////  root["volt"] ="220.10";
////  root["ampere"] ="32.10";
////  root["watt"] ="20.10";
////  root["kwh"] ="18.10";
  String pubmsg;
  root.printTo(pubmsg);
  Serial.println(pubmsg);
  Serial.println(MAC_char);
  client.publish(routingkey1, pubmsg.c_str());
//  delay(5000)
//  client.publish(routingkey2, pubmsg.c_str());
  }
