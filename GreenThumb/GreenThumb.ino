//***********************************************************************//
//Name: Green Thumb
//Version: 1.1.0
//Author: Ryan Timmerman
//Date: 12/22/2022
//Description: This script is designed to automate an indoor plant biome.
//***********************************************************************//

#include "SparkFun_SGP30_Arduino_Library.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 7
#define DHTTYPE DHT11

#define humidifierRelay 2
#define fanRelay 4

SGP30 sgp;

DHT dht(DHTPIN, DHTTYPE);
int loopCtr = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);

  if (sgp.begin() == false)  //Initialize co2 sensor
  {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1)
      ;
  }
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  sgp.initAirQuality();

  pinMode(fanRelay, OUTPUT);
  pinMode(humidifierRelay, OUTPUT);

  digitalWrite(fanRelay, HIGH);         //turn relay off
  digitalWrite(humidifierRelay, HIGH);  //turn relay off

  dht.begin();  //Initialize humid/temp sensor

  setupDisplay();

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
  sgp.measureAirQuality();

  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();
  float cO2 = sgp.CO2;
  float tVOC = sgp.TVOC;

  // Serial.print("CO2: ");
  // Serial.print(cO2);
  // Serial.print(" ppm\tTVOC: ");
  // Serial.print(tVOC);
  // Serial.print(" ppb\t");
  // Serial.print("RH: ");
  // Serial.print(humid);
  // Serial.print("TEMP: ");
  // Serial.println(tempC);

  delay(1000);

  updateDisplay(humid, tempC, cO2, tVOC);

  // //If RH is lower than 85% trigger humidfier
  // // if (humid < 85) {
  // //   humidfy();
  // // }

  // //every 60 loops (2mins x times humidifer triggered + 30 minutes)
  // //Turn on the external fans for fresh air exchange
  // // if (loopCtr >= 60) {
  // //   exchangeFreshAir();
  // //   loopCtr = 0;  // reset the counter
  // // }

  //  loopCtr++;  //add 1 to the counter

  // // //wait 30 seconds before looping
  // // delay(30000);

  delay(1000);
  loopCtr++;
}


void initializeTestSequence() {
  // Serial.println("");
  // Serial.println("*************************************************");
  Serial.println("Green Thumb 1.0 Starting..");

  //turn fans on for 10 seconds
  // Serial.println("testing fans..");
  // digitalWrite(fanRelay, LOW);   //turn fan relay on
  // delay(10000);                  //wait 10 seconds
  // digitalWrite(fanRelay, HIGH);  //turn fan relay off
  // delay(250);

  // //turn humidifier on for 10 seconds
  // Serial.println("testing humidifier..");
  // digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  // delay(10000);                         //wait 10 seconds
  // digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  // delay(250);

  delay(1000);
}

void updateDisplay(int humidity, int tempC, int co2, int tvoc) {
  
  int tempF = 1.8 * tempC + 32;
  String tempString1 = "TEMP ";
  String tempString2 = String(tempF);
  String tempString3 = "F";

  String finalTempString = tempString1 + tempString2 + tempString3;

  String humidString = "RH: " + String(humidity) + "%";

  Serial.println(finalTempString);
  Serial.println(humidString);

  // Serial.println("Temp:" + tempString + "    RH:" + humidString);  //+ airQualityString);


  // int airQualityString_len = airQualityString.length() + 1; // Length (with one extra character for the null terminator)
  // char aq_char_array[airQualityString_len];
  // airQualityString.toCharArray(aq_char_array, airQualityString_len);

  int tempString_len = finalTempString.length() + 1;  // Length (with one extra character for the null terminator)
  char temp_char_array[tempString_len];
  finalTempString.toCharArray(temp_char_array, tempString_len);

  int humidString_len = humidString.length() + 1;  // Length (with one extra character for the null terminator)
  char humid_char_array[humidString_len];
  humidString.toCharArray(humid_char_array, humidString_len);


String airQualityString = String(sgp.CO2) + "ppm";


  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("CO2: ");
  display.setTextSize(2);
  display.print(String(sgp.CO2));
  display.setTextSize(1);
  display.println("ppm");
  display.setTextSize(2);
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
  delay(120000);                        //wait 120 seconds (2mins)
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