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

#define PHOTO 4

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
void displayDark(float t, float h, float p, float a, float hi, int soil);
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

int photo = analogRead(PHOTO);
bool isDarkMode = false; // Flag to keep track of the display mode

void setup() {
    Serial.begin(9600);
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
    if (isDarkMode) {
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);
    } else {
        tft.fillScreen(ILI9341_WHITE);
        tft.setTextColor(ILI9341_BLACK);
    }
    tft.setCursor(0, 0);
    tft.setTextSize(2);

  pinMode(BTN3, INPUT_PULLUP); // Configure BTN3 as input with internal pull-up resistor

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
    bool published = client.publish(pubtopic, doc);
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

    Serial.printf("Photoresistor: %d\n", photo);

    // Check if BTN3 is pressed
    if (digitalRead(BTN3) == LOW) {
      isDarkMode = !isDarkMode; // Toggle the display mode
      delay(200); // Debounce delay
    }

    // if(photo>500){
    //   isDarkMode = !isDarkMode; // Toggle the display mode
    // }
    // // Display readings or dark mode based on the flag
    // if (isDarkMode) {
    //     displayDark(t, h, p, a, hi, soil);
    // } else {
        displayReadings(t, h, p, a, hi, soil);
    // }

    delay(2000);
}

void displayReadings(float t, float h, float p, float a, float hi, int soil)
{
    // Define label positions
    int startX = 10;
    int startY = 10;
    int yGap = 50;
    int rectHeight = 40; // Adjust as needed to fit two lines of text
    int rectWidth = 300; // Adjust as needed

    // Clear the screen with a white background
    //tft.fillScreen(ILI9341_WHITE);

    // Function to draw text with dividers
    auto drawTextWithDivider = [&](int x, int y, const char* label, float value, const char* unit) {
        // Set text color and size
        tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
        tft.setTextSize(2); // Set text size to 2

        // Print the label
        tft.setCursor(x, y);
        tft.print(label);

        // Print the value on the next line
        tft.setCursor(x, y + 20);
        tft.print(value, 2);  // Value rounded to 2 decimal places
        tft.print(" ");
        tft.println(unit);
    };

    // Draw text with dividers
    drawTextWithDivider(startX, startY, "Temperature: ", t, "C");
    tft.drawLine(startX, startY + rectHeight, startX + rectWidth, startY + rectHeight, ILI9341_BLACK);

    drawTextWithDivider(startX, startY + yGap, "Humidity: ", h, "%");
    tft.drawLine(startX, startY + yGap + rectHeight, startX + rectWidth, startY + yGap + rectHeight, ILI9341_BLACK);

    drawTextWithDivider(startX, startY + 2 * yGap, "Pressure: ", p, "Pa");
    tft.drawLine(startX, startY + 2 * yGap + rectHeight, startX + rectWidth, startY + 2 * yGap + rectHeight, ILI9341_BLACK);

    drawTextWithDivider(startX, startY + 3 * yGap, "Altitude: ", a, "m");
    tft.drawLine(startX, startY + 3 * yGap + rectHeight, startX + rectWidth, startY + 3 * yGap + rectHeight, ILI9341_BLACK);

    // Convert soil reading
    float soilPercentage = 100 - ((soil / 4095.0) * 100.0);
    drawTextWithDivider(startX, startY + 4 * yGap, "Soil: ", soilPercentage, "%");
    tft.drawLine(startX, startY + 4 * yGap + rectHeight, startX + rectWidth, startY + 4 * yGap + rectHeight, ILI9341_BLACK);

    drawTextWithDivider(startX, startY + 5 * yGap, "Heat Index: ", hi, "");
    tft.drawLine(startX, startY + 5 * yGap + rectHeight, startX + rectWidth, startY + 5 * yGap + rectHeight, ILI9341_BLACK);
}

void displayDark(float t, float h, float p, float a, float hi, int soil)
{
    // Define label positions
    int startX = 10;
    int startY = 10;
    int yGap = 50;
    int rectHeight = 40; // Adjust as needed to fit two lines of text
    int rectWidth = 300; // Adjust as needed

    // Clear the screen with a white background
    //tft.fillScreen(ILI9341_WHITE);

    // Function to draw text with dividers
    auto drawTextWithDivider = [&](int x, int y, const char* label, float value, const char* unit) {
        // Set text color and size
        tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
        tft.setTextSize(2); // Set text size to 2

        // Print the label
        tft.setCursor(x, y);
        tft.print(label);

        // Print the value on the next line
        tft.setCursor(x, y + 20);
        tft.print(value, 2);  // Value rounded to 2 decimal places
        tft.print(" ");
        tft.println(unit);
    };

    // Draw text with dividers
    drawTextWithDivider(startX, startY, "Temperature: ", t, "C");
    tft.drawLine(startX, startY + rectHeight, startX + rectWidth, startY + rectHeight, ILI9341_WHITE);

    drawTextWithDivider(startX, startY + yGap, "Humidity: ", h, "%");
    tft.drawLine(startX, startY + yGap + rectHeight, startX + rectWidth, startY + yGap + rectHeight, ILI9341_WHITE);

    drawTextWithDivider(startX, startY + 2 * yGap, "Pressure: ", p, "Pa");
    tft.drawLine(startX, startY + 2 * yGap + rectHeight, startX + rectWidth, startY + 2 * yGap + rectHeight, ILI9341_WHITE);

    drawTextWithDivider(startX, startY + 3 * yGap, "Altitude: ", a, "m");
    tft.drawLine(startX, startY + 3 * yGap + rectHeight, startX + rectWidth, startY + 3 * yGap + rectHeight, ILI9341_WHITE);

    // Convert soil reading
    float soilPercentage = 100 - ((soil / 4095.0) * 100.0);
    drawTextWithDivider(startX, startY + 4 * yGap, "Soil: ", soilPercentage, "%");
    tft.drawLine(startX, startY + 4 * yGap + rectHeight, startX + rectWidth, startY + 4 * yGap + rectHeight, ILI9341_WHITE);

    drawTextWithDivider(startX, startY + 5 * yGap, "Heat Index: ", hi, "");
    tft.drawLine(startX, startY + 5 * yGap + rectHeight, startX + rectWidth, startY + 5 * yGap + rectHeight, ILI9341_WHITE);
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