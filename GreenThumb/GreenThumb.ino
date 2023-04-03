//***********************************************************************//
//Name: Green Thumb
//Version: 1.2.0
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
  Wire.begin();
  Serial.begin(115200);

  pinMode(fanRelay, OUTPUT);
  pinMode(humidifierRelay, OUTPUT);

  digitalWrite(fanRelay, HIGH);         //turn relay off
  digitalWrite(humidifierRelay, HIGH);  //turn relay off

  dht.begin();  //Initialize humid/temp sensor

  if (sgp.begin() == false)  //Initialize co2 sensor
  {
    Serial.println("No SGP30 Detected. Check connections.");
    while (1)
      ;
  }
  //Initializes sensor for air quality readings
  //measureAirQuality should be called in one second increments after a call to initAirQuality
  sgp.initAirQuality();

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

  updateDisplay(humid, tempC, cO2, tVOC);

  //If RH is lower than 85% trigger humidfier
  if (humid < 85) {
    humidfy();
  }

  if (cO2 > 1500) {
    exchangeFreshAir();
  }

  delay(1000);
  loopCtr++;
}


void initializeTestSequence() {
  Serial.println("Green Thumb 1.0 Starting..");

  //turn fans on for 3 seconds
  Serial.println("testing fans..");
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(3000);                  //wait 10 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
  delay(250);

  // //turn humidifier on for 3 seconds
  Serial.println("testing humidifier..");
  digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  delay(3000);                         //wait 10 seconds
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  delay(250);

  delay(1000);
}

void updateDisplay(int humidity, int tempC, int co2, int tvoc) {
  int tempF = 1.8 * tempC + 32;

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
  display.print("TEMP ");
  display.print(String(tempF));
  display.println("F");
  display.setCursor(10, 45);
  display.print("RH: ");
  display.print(String(humidity));
  display.println("%");
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
}

void exchangeFreshAir() {
  Serial.println("FRESH AIR: Power on fans..");
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(60000);                  //wait 60 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
}