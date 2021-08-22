#include <ArduinoJson.h>
void setup() {
  Serial.begin(9600);
 
}

void loop() {
 sensor(); 
}
void sensor(){
//  data sensor ini di ambli dari sensor cuma ini cuma contoh aja
String Sensor1="12";
String Sensor2="13";
String Sensor3="14";

//prosess jadiin json
//  float unixtime = ntpClient.getUnixTime();
//  Serial.println(unixtime);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["sensor1"] = Sensor1;
  root["sensor2"] =Sensor2;
  root["sensor3"] =Sensor3;
//  udah jadi json
  String pubmsg;
  root.printTo(pubmsg);
//  print json
  Serial.println(pubmsg);
  }
