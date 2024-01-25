#include <GxEPD.h>
#include <GxGDEW042T2/GxGDEW042T2.h>  // 4.2" b/w display
#include <Fonts/FreeMonoBold18pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include "DHT.h"  // DHT sensor library

// Pin configuration for ESP32 and other boards
#if defined(ESP32)
const int CS_PIN = 5;
const int DC_PIN = 17;
const int RST_PIN = 16;
const int BUSY_PIN = 4;
const int TOUCH_GPIO = 13;
const int DHTPIN = 27;
#else
const int CS_PIN = SS;
const int DC_PIN = 8;
const int RST_PIN = 9;
const int BUSY_PIN = 7;
const int TOUCH_GPIO = 13;
const int DHTPIN = 27;
#endif

// Display and sensor initialization
GxIO_Class io(SPI, CS_PIN, DC_PIN, RST_PIN);
GxEPD_Class display(io, RST_PIN, BUSY_PIN);
DHT dht(DHTPIN, DHT11);

// Global variables for sensor readings
float prevTemp = -1.0;
float prevHumidity = -1.0;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing display and sensors");

  display.init(115200);  // Diagnostic output on Serial
  dht.begin();

  Serial.println("Setup complete");
}

void loop() {
  readAndDisplaySensorData();
  delay(2000);  // 2-second delay
}

void readAndDisplaySensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.printf("Temperature: %f; Humidity: %f\n", temperature, humidity);

  if (temperature != prevTemp || humidity != prevHumidity) {
    displaySensorDataOnScreen(temperature, humidity);
    prevTemp = temperature;
    prevHumidity = humidity;
  }
}

void displaySensorDataOnScreen(float temperature, float humidity) {
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&FreeMonoBold18pt7b);
  display.setTextColor(GxEPD_BLACK);

  // Prepare the text to display
  String tempText = "Temp: " + String(temperature, 2);
  String humidityText = "Humidity: " + String(humidity, 2);

  // Calculate width and height of the text block
  int16_t tbx, tby;  // text box x and y position
  uint16_t tbw, tbh; // text box width and height
  display.getTextBounds(tempText, 0, 0, &tbx, &tby, &tbw, &tbh);
  int totalHeight = tbh * 2; // Two lines of text

  // Calculate starting position for the text so it's centered
  int x = (display.width() - tbw) / 2;
  int y = (display.height() - totalHeight) / 2;

  display.setCursor(x, y);
  display.println(tempText);
  display.setCursor(x, y + tbh); // Move cursor down for the next line
  display.println(humidityText);

  display.update();
  display.powerDown();
}

