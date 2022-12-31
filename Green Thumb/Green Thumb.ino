//***********************************************************************//
//Name: Green Thumb
//Version: 1.0.0
//Author: Ryan Timmerman
//Date: 12/22/2022
//Description: This script is designed to automate an indoor plant biome.
//***********************************************************************//

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN A0
#define DHTTYPE DHT11

// #define fanRelay 2
// #define humidifierRelay 4

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(115200);

  // pinMode(fanRelay, OUTPUT);
  // pinMode(humidifierRelay, OUTPUT);

  setupDisplay();

  dht.begin();  //initiate humid/temp sensor

  // initializeTestSequence();
}

void setupDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("GreenThumb");
  display.display();
  display.setTextSize(2);
}

void loop() {

  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();

  updateDisplay(humid, tempC);

  // if (humid < 80) {
  //   //turn fan and humidifer on for 10 seconds
  //   Serial.println("LOW HUMIDITY: Powering on fan and humidifier..");

  //   digitalWrite(fanRelay, LOW);
  //   digitalWrite(humidifierRelay, LOW);

  //   delay(10000);

  //   digitalWrite(fanRelay, HIGH);         //turn relays off
  //   digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  // }

  //wait 10 sec before looping
  delay(10000);
}

// void initializeTestSequence() {
//   Serial.println("");
//   Serial.println("*************************************************");
//   Serial.println("Green Thumb 1.0 Starting..");

//   //turn relays on for 3 seconds w/ .25s between relays
//   digitalWrite(fanRelay, LOW);
//   delay(250);
//   digitalWrite(humidifierRelay, LOW);

//   delay(3000);  //wait 3 seconds

//   digitalWrite(fanRelay, HIGH);  //turn relays off
//   delay(250);
//   digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
// }

void updateDisplay(int humidity, int tempC) {
  String tempString = "TEMP " + String((int)(1.8 * tempC + 32)) + "F";
  int tempString_len = tempString.length() + 1;  // Length (with one extra character for the null terminator)
  char temp_char_array[tempString_len];
  tempString.toCharArray(temp_char_array, tempString_len);

  String humidString = "RH: " + String(humidity) + "%";
  int humidString_len = humidString.length() + 1;  // Length (with one extra character for the null terminator)
  char humid_char_array[humidString_len];
  humidString.toCharArray(humid_char_array, humidString_len);

  Serial.println(tempString + "       " + humidString);

  display.setCursor(0,25);
  display.println(temp_char_array);
  display.setCursor(10,45);
  display.println(humid_char_array);
  display.invertDisplay(true);
  display.display();
  delay(1000);

  display.invertDisplay(false);
  delay(1000);
}