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


#define DHTPIN 7
#define DHTTYPE DHT11

#define humidifierRelay 2
#define fanRelay 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
SGP30 sgp;
DHT dht(DHTPIN, DHTTYPE);
uint8_t loopCtr;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(fanRelay, OUTPUT);
  pinMode(humidifierRelay, OUTPUT);

  digitalWrite(fanRelay, HIGH);         //turn relay off
  digitalWrite(humidifierRelay, HIGH);  //turn relay off

  dht.begin();  //Initialize humid/temp sensor
  delay(500);

  if (sgp.begin() == false)  //Initialize co2 sensor
  {
    Serial.println(F("No SGP30 Detected. Check connections."));
    while (1)
      ;
  }

  sgp.initAirQuality();  //Initializes sensor for air quality readings
  delay(500);
  
  setupDisplay();
  delay(500);

  initializeTestSequence();
  delay(500);
}

void initializeTestSequence() {
  Serial.println("Green Thumb 1.2 Starting..");

  //turn fans on for 3 seconds
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(3000);                   //wait 3 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
  delay(250);

  // //turn humidifier on for 3 seconds
  digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  delay(3000);                          //wait 3 seconds
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  delay(250);

  updateSgpHumidity();
}

void loop() {
  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();

  //Every 60 loops update the SGP sensor's humidity value
  if (loopCtr > 59) {
    updateSgpHumidity();
    loopCtr = 0;
  }

  sgp.measureAirQuality();
  delay(500);

  float cO2 = sgp.CO2;
  

  updateDisplay(humid, tempC, cO2);

  if (humid < 85) {
    humidfy();
  }

  if (cO2 > 1200) {
    exchangeFreshAir();
  }

  delay(5000);
  loopCtr++;
}

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (1)
      ;
  }
  delay(250);
  display.clearDisplay();
  delay(250);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println(F("GreenThumb"));
  display.setCursor(20, 25);
  display.println(F("v1.2"));
  display.setCursor(0, 45);
  display.println(F("starting.."));
  display.display();
}

void updateSgpHumidity() {
  float humid = dht.readHumidity();
  float tempC = dht.readTemperature();

  double absHumidity = RHtoAbsolute(humid, tempC);  //Convert relative humidity to absolute humidity
  uint16_t sensHumidity = doubleToFixedPoint(absHumidity);

  sgp.setHumidity(sensHumidity);  //Set humidity compensation on the SGP30
  delay(500);

  //the first 15 CO2 readings will be 400ppm
  for (int i = 0; i < 15; i++) {
    sgp.measureAirQuality();
    delay(1000);
  }
}

void updateDisplay(const int& humidity, const int& tempC, const int& co2) {
  int tempF = 1.8 * tempC + 32;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("CO2: "));
  display.setTextSize(2);
  display.print(String(co2));
  display.setTextSize(1);
  display.print(F("ppm  "));
  display.println(String(loopCtr));
  display.setTextSize(2);
  display.setCursor(0, 25);
  display.print(F("TEMP "));
  display.print(String(tempF));
  display.println(F("F"));
  display.setCursor(10, 45);
  display.print(F("RH: "));
  display.print(String(humidity));
  display.println(F("%"));
  display.invertDisplay(true);
  display.display();
  delay(500);
  display.invertDisplay(false);
  delay(500);
}

void humidfy() {
  digitalWrite(humidifierRelay, LOW);   //turn humidifer relay on
  delay(120000);                        //wait 120 seconds (2mins)
  digitalWrite(humidifierRelay, HIGH);  //turn humidifer relay off
  delay(500);
}

void exchangeFreshAir() {
  digitalWrite(fanRelay, LOW);   //turn fan relay on
  delay(60000);                  //wait 60 seconds
  digitalWrite(fanRelay, HIGH);  //turn fan relay off
  delay(500);
}

double RHtoAbsolute(const float& relHumidity, const float& tempC) {
  double eSat = 6.11 * pow(10.0, (7.5 * tempC / (237.7 + tempC)));
  double vaporPressure = (relHumidity * eSat) / 100;                          //millibars
  double absHumidity = 1000 * vaporPressure * 100 / ((tempC + 273) * 461.5);  //Ideal gas law with unit conversions
  return absHumidity;
}

uint16_t doubleToFixedPoint(const double& number) {
  int power = 1 << 8;
  double number2 = number * power;
  uint16_t value = floor(number2 + 0.5);
  return value;
}