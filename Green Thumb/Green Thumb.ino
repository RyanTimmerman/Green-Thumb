//***********************************************************************//
//Name: Green Thumb
//Version: 1.0.0
//Author: Ryan Timmerman
//Date: 12/22/2022
//Description: This script is designed to automate an indoor plant biome.
//***********************************************************************//

// #include <Wire.h>
// #include "ACROBOTIC_SSD1306.h"
#include "DHT.h"

#define DHTPIN A0
#define DHTTYPE DHT11
#define fanRelay 2
#define humidifierRelay 4

DHT dht(DHTPIN, DHTTYPE);
int loopCtr = 0;

void setup() {

  Serial.begin(9600);
  pinMode(fanRelay, OUTPUT);
  pinMode(humidifierRelay, OUTPUT);

  // setupDisplay();
  dht.begin();  //initiate humid/temp sensor

  initializeTestSequence();
}

// void setupDisplay() {
//   Wire.begin();
//   oled.init();           // Initialze SSD1306 OLED display
//   oled.clearDisplay();   // Clear screen
//   oled.setTextXY(0, 0);  // Set cursor position, start of line 0
//   oled.putString("Green Thumb 1.0");
// }

void loop() {

  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();

  updateDisplay(humid, tempC);

  if (humid < 80) {
    //turn fan and humidifer on for 5 seconds
    Serial.println("LOW HUMIDITY: Powering on fan and humidifier..");

    digitalWrite(fanRelay, LOW);
    digitalWrite(humidifierRelay, LOW);

    delay(5000);

    digitalWrite(fanRelay, HIGH);         //turn relays off
    digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  }

  //every 4 hours turn the fan on for 10 seconds
  if (loopCtr == 240) {
    loopCtr = 0;
    Serial.println("FRESH AIR: Powering on fan..");

    digitalWrite(fanRelay, LOW);
    delay(10000);
    digitalWrite(fanRelay, HIGH);  //turn relays off
  }

  loopCtr++;

  //wait 1 minute before looping
  delay(60000);
}

void initializeTestSequence() {
  Serial.println("Green Thumb 1.0 Starting..");

  //turn relays on for 3 seconds w/ .25s between relays
  digitalWrite(fanRelay, LOW);
  delay(250);
  digitalWrite(humidifierRelay, LOW);

  delay(3000);  //wait 3 seconds

  digitalWrite(fanRelay, HIGH);  //turn relays off
  delay(250);
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
}

void updateDisplay(float humidity, float tempC) {
  String tempString = "Temp: " + String((float)(1.8 * tempC + 32)) + " F";
  //   int tempString_len = tempString.length() + 1;  // Length (with one extra character for the null terminator)
  //   char temp_char_array[tempString_len];
  //   tempString.toCharArray(temp_char_array, tempString_len);

  String humidString = "Humidity: " + String(humidity) + "%";
  //   int humidString_len = humidString.length() + 1;  // Length (with one extra character for the null terminator)
  //   char humid_char_array[humidString_len];
  //   humidString.toCharArray(humid_char_array, humidString_len);

  Serial.println(tempString + "       " + humidString);

  //   oled.setTextXY(1, 0);
  //   oled.putString(temp_char_array);

  //   oled.setTextXY(2, 0);
  //   oled.putString(humid_char_array);
}