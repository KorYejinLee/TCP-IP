#include "DHT.h"
enum CONTROL_PINS {
  TEMPER_HUMID = A0,
  RED_LED = 8U,
  YELLOW_LED,
  BLUE_PIN,
  GREEN_PIN,
  RED_PIN
};
class DHT dht(TEMPER_HUMID, 11);
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(115200UL);
  //Serial1.begin(9600UL); // BLUETOOTH
  pinMode(TEMPER_HUMID,INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int vr_value = analogRead(TEMPER_HUMID);
  int mapped_value = map(vr_value, 0, 1023, 0, 255);
  if(dht.read()) {
    const float temperature {dht.readTemperature()};
    const float humidity {dht.readHumidity()};
    if(Serial.available()) {
      String in_comming_string {Serial.readStringUntil('\n')}; // new line이 올때까지 읽어드려라
      in_comming_string.trim();
      int index = in_comming_string.lastIndexOf('_'); // \r,=n 잘라낸다
      if(in_comming_string.substring(0,index+1).equals("RED_LED_ON_")) {
        analogWrite(RED_PIN, in_comming_string.substring(index+1,in_comming_string.length()).toInt());
      } else if(in_comming_string.equals("RED_LED_OFF")) {
        analogWrite(RED_PIN, 0);
      } else if(in_comming_string.substring(0,index+1).equals("BLUE_LED_ON_")) {
        analogWrite(BLUE_PIN, in_comming_string.substring(index+1,in_comming_string.length()).toInt());
      } else if(in_comming_string.equals("BLUE_LED_OFF")) {
        analogWrite(BLUE_PIN, 0);
      } else {}
    }
    const String sending_data {String(temperature) + ',' + String(humidity)}; // csv 타입
    Serial.println(sending_data);
    Serial.print("Map VALUE : ");
    Serial.println(mapped_value);
    delay(1000UL); // delay(500UL);
  }
}
