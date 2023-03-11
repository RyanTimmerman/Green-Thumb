//***********************************************************************//
//Name: Green Thumb
//Version: 1.1.0
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

#define humidifierRelay 2
#define fanRelay 4

DHT dht(DHTPIN, DHTTYPE);
int loopCtr = 0;

void setup() {

  Serial.begin(115200);

  pinMode(fanRelay, OUTPUT);
  pinMode(humidifierRelay, OUTPUT);

  digitalWrite(fanRelay, HIGH);         //turn relay off
  digitalWrite(humidifierRelay, HIGH);  //turn relay off

  setupDisplay();

  dht.begin();  //initiate humid/temp sensor

  initializeTestSequence();
}

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(250);
  display.clearDisplay();
  delay(250);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("GreenThumb");
  display.setCursor(0, 25);
  display.println("starting..");
  display.display();
}


void loop() {

  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();

  updateDisplay(humid, tempC);

  //If RH is lower than 80% trigger humidfier
  if (humid < 80) {
    humidfy();
  }

  //every 60 loops (2mins x times humidifer triggered + 30 minutes)
  //Turn on the external fans for fresh air exchange
  if (loopCtr >= 60) {
    exchangeFreshAir();
    loopCtr = 0;  // reset the counter
  }

  loopCtr++;  //add 1 to the counter

  //wait 30 seconds before looping
  delay(30000);
}

void initializeTestSequence() {
  Serial.println("");
  Serial.println("*************************************************");
  Serial.println("Green Thumb 1.0 Starting..");

  //turn fans on for 10 seconds
  Serial.println("testing fans..");
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(10000);                  //wait 10 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
  delay(250);

  //turn humidifier on for 10 seconds
  Serial.println("testing humidifier..");
  digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  delay(10000);                         //wait 10 seconds
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  delay(250);
}

void updateDisplay(int humidity, int tempC) {
  String tempString = "TEMP " + String((int)(1.8 * tempC + 32)) + "F";
  String humidString = "RH: " + String(humidity) + "%";
  Serial.println(tempString + "    " + humidString);

  int tempString_len = tempString.length() + 1;  // Length (with one extra character for the null terminator)
  char temp_char_array[tempString_len];
  tempString.toCharArray(temp_char_array, tempString_len);

  int humidString_len = humidString.length() + 1;  // Length (with one extra character for the null terminator)
  char humid_char_array[humidString_len];
  humidString.toCharArray(humid_char_array, humidString_len);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("GreenThumb");
  display.setCursor(0, 25);
  display.println(temp_char_array);
  display.setCursor(10, 45);
  display.println(humid_char_array);
  display.invertDisplay(true);
  display.display();
  delay(500);
  display.invertDisplay(false);
  delay(500);
}

void humidfy() {
  Serial.println("LOW HUMIDITY: Powering on humidifier..");
  digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  delay(120000);                         //wait 120 seconds (2mins)
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  delay(250);

  // //turn the external fans on distrubute moisture evenly
  // digitalWrite(fanRelay, LOW);   //turn fan relay on
  // delay(10000);                  //wait 10 seconds
  // digitalWrite(fanRelay, HIGH);  //turn fan relay off
  // delay(250);
}

void exchangeFreshAir() {
  Serial.println("FRESH AIR: Power on fans..");
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(60000);                  //wait 60 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
}