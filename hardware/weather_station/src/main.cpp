#include <Arduino.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

#ifndef _WIFI_H
#include <WiFi.h>
#include <HTTPClient.h>
#endif

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#include <stdio.h>
#endif

#include <rom/rtc.h>

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

WiFiClient espClient;
PubSubClient client(espClient);

// DEFINE VARIABLES
#define dhtPin 32
#define dhtType DHT22
#define soilpin 35

#define analogPin 14
#define BTN1 25
#define BTN2 26
#define BTN3 27

#define TFT_DC 17
#define TFT_CS 5
#define TFT_RST 16
#define TFT_CLK 18
#define TFT_MOSI 23
#define TFT_MISO 19

#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

// Function declaration
bool checkDHT(float t, float h);
String toDocument(float t, float h, float p, float a, float hi, int soil);
void displayReadings(float t, float h, float p, float a, float hi, int soil);
float calculateHeatIndex(float t, float h);
String JsonDoc(float t, float h, float p, float a, int soil, float hi);

// MQTT CLIENT CONFIG
static const char* pubtopic      = "620155671";                    // Add your ID number here
static const char* subtopic[]    = {"620155671_sub","/elet2415"};  // Array of Topics(Strings) to subscribe to
const char *ssid = "MonaConnect";
const char *password = "";

static const char *mqtt_server = "broker.emqx.io";       // Broker IP address or Domain name as a String
static uint16_t mqtt_port = 1883;


Adafruit_BMP280 bmp; // I2C
DHT dht(dhtPin, dhtType);

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    if (WiFi.isConnected()) {
        Serial.println("Connected to wifi network");
    }

    dht.begin();
    while (!bmp.begin(0x76)) {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        delay(1000);
    }
    // Set up the display
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);


  client.setServer(mqtt_server, mqtt_port);
    while(!client.connect("demo")){
    Serial.println("Connecting to Server...");
        delay(5000);
  }
}
 
void loop() {
    // Read values from the hardware
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    float p = bmp.readPressure();
    float a = bmp.readAltitude(1014.8);
    int soil = analogRead(soilpin);

    // Calculate Heat Index
    float hi = calculateHeatIndex(t, h);

    const char *doc = JsonDoc(t,h,p,a,soil,hi).c_str();
    Serial.println(doc);
    bool published = client.publish("demo", doc);
     if(published)
    Serial.println("Published");

    // Check if any reads failed and exit early (to try again).
    if (!checkDHT(t, h)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Print the sensor readings
    Serial.printf("\nTemperature: %f\n", t);
    Serial.printf("Humidity: %f\n", h);
    Serial.printf("Pressure: %f\n", p);
    Serial.printf("Altitude: %f\n", a);
    Serial.printf("Soil: %d\n", soil);
    Serial.printf("Heat Index: %f\n", hi);

    displayReadings(t, h, p, a, hi, soil);

    delay(2000);
}

void displayReadings(float t, float h, float p, float a, float hi, int soil)
{
    // Print labels
    tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
    tft.setTextSize(2);

    // Define label positions
    int startX = 10;
    int startY = 10;
    int yGap = 50;
    int rectHeight = 20; // Adjust as needed

    // Clear area and Print Temperature
    //tft.fillRect(startX, startY, 300, rectHeight, ILI9341_WHITE);
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);

    tft.setCursor(startX, startY);
    tft.print("Temperature: ");
    tft.print(t, 2);  // Value rounded to 2 decimal places
    tft.println(" C"); // Unit

    // Clear area and Print Humidity
    //tft.fillRect(startX, startY + yGap, 300, rectHeight, ILI9341_BLACK);
    tft.setCursor(startX, startY + yGap);
    tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);

    tft.print("Humidity: ");
    tft.print(h, 2);  // Value rounded to 2 decimal places
    tft.println(" %"); // Unit

    // Clear area and Print Pressure
   // tft.fillRect(startX, startY + 2 * yGap, 300, rectHeight, ILI9341_BLACK);
    tft.setCursor(startX, startY + 2 * yGap);
    tft.setTextColor(ILI9341_PINK,ILI9341_BLACK);

    tft.print("Pressure: ");
    tft.print(p, 2);  // Value rounded to 2 decimal places
    tft.println(" Pa"); // Unit

    // Clear area and Print Altitude
    //tft.fillRect(startX, startY + 3 * yGap, 300, rectHeight, ILI9341_BLACK);
    tft.setCursor(startX, startY + 3 * yGap);
    tft.setTextColor(ILI9341_CYAN,ILI9341_BLACK);

    tft.print("Altitude: ");
    tft.print(a, 2);  // Value rounded to 2 decimal places
    tft.println(" m"); // Unit

    // Convert soil reading
    float soilPercentage = 100 -((soil / 4095.0) * 100.0);

    // Clear area and Print Soil
    //tft.fillRect(startX, startY + 4 * yGap, 300, rectHeight, ILI9341_BLACK);
    tft.setCursor(startX, startY + 4 * yGap);
    tft.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);

    tft.print("Soil: ");
    tft.print(soilPercentage, 2);  // Value rounded to 2 decimal places
    tft.println(" %"); // Unit

    // Clear area and Print Heat Index
    //tft.fillRect(startX, startY + 5 * yGap, 300, rectHeight, ILI9341_BLACK);
    tft.setCursor(startX, startY + 5 * yGap);
    tft.setTextColor(ILI9341_ORANGE,ILI9341_BLACK);

    tft.print("Heat Index: ");
    tft.print(hi, 2);  // Value rounded to 2 decimal places
    tft.println(" C"); // Unit
}

bool checkDHT(float t, float h)
{
    if (isnan(t) || isnan(h))
    {
        return false;
    }
    return true;
}

float calculateHeatIndex(float t, float h)
{
    // Constants for the Steadman Heat Index formula
    const float c1 = -8.4157;
    const float c2 = 1.9243;
    const float c3 = 0.5204;
    const float c4 = -0.0261;
    
    // Calculate the Heat Index
    float hi = c1 + c2*t + c3*h + c4*t*h;

    return hi;
}

  String JsonDoc(float t, float h, float p, float a, int soil, float hi){
    JsonDocument doc;
    doc["temperature"] = ceil(t * 100.0)/100.0;
    doc["humidity"] = ceil(h * 100.0)/100.0;
    doc["pressure"] = ceil(p * 100.0)/100.0;
    doc["altitude"] = ceil(a * 100.0)/100.0;
    doc["soil"] = ceil(soil * 100.0)/100.0;
    doc["heatindex"] = ceil(hi * 100.0)/100.0;
    String output;
    serializeJson(doc, output);
    return output;

  }