/*
  LoRa Duplex communication with Sync Word

  Sends a message every half second, and polls continually
  for new incoming messages. Sets the LoRa radio's Sync Word.

  The Sync Word is basically the radio's network ID. Radios with different
  Sync Words will not receive each other's transmissions. This is one way you
  can filter out radios you want to ignore, without making an addressing scheme.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on Sync Word.

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define LORA_CS 18
#define LORA_RESET 23
#define LORA_DIO0 26
#define LORA_FREQ 433E6

#define OLED_SDA 21
#define OLED_SCL 22 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// Checkout https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDRESS 0x3C // See https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;

char ssid[23];

void setup() {
  Serial.begin(115200);

   // Get deviceId
  snprintf(ssid, 23, "MCUDEVICE-%llX", ESP.getEfuseMac());

  Wire.begin(OLED_SDA, OLED_SCL);

  //initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  Serial.println("Looking for sensor");
  if (! bme.begin(BME280_ADDRESS_ALTERNATE)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  display.setCursor(5,10);
  display.print("BME280 found");

  LoRa.setPins(LORA_CS, LORA_RESET, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  display.setCursor(5,25);
  display.print("LORA OK");
  display.setCursor(5,40);
  display.print("Sampling started");
  display.setCursor(5,55);
  display.print(String(ssid).substring(10));
  
  display.display();

  delay(2500);

  LoRa.setPreambleLength(8);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);

  // weather monitoring
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
  Serial.println("filter off");

  // suggested rate is 1/60Hz (1m)
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1, // temperature
                  Adafruit_BME280::SAMPLING_X1, // pressure
                  Adafruit_BME280::SAMPLING_X1, // humidity
                  Adafruit_BME280::FILTER_OFF   );
                    
}

void sendMessage(String outgoing);
void displayAndSendBmeValues();

void loop() {

    bme.takeForcedMeasurement();
    
    displayAndSendBmeValues();

    // The sensor should be read every 60 seconds when in Forced mode, according to the datasheet and Adafruit library
    delay(60000);
}


void displayAndSendBmeValues() {

    String temp = String(bme.readTemperature());
    String pressure = String(bme.readPressure() / 100);
    String humidity = String(bme.readHumidity());
    String altitude = String(bme.readAltitude(SEALEVELPRESSURE_HPA));

    display.clearDisplay();
    display.setCursor(10,10);
    display.print("Temp:  " + temp + " C");
    display.setCursor(10,25);
    display.print("Press: " + pressure + " hPa");
    display.setCursor(10,40);
    display.print("Hum:   " + humidity + " %");
    display.setCursor(10,55);
    display.print("Alt:   " + altitude + " m");
    display.display();

    String payload = "";
    payload += "{\"temperature\":";
    payload += temp;
    payload += ",\"pressure\":";
    payload += pressure;
    payload += ",\"humidity\":";
    payload += humidity;
    payload += ",\"altitude\":";
    payload += altitude;
    payload += ",\"sensor\":";
    payload += "\"bme280\"";
    payload += ",\"device\":";
    payload += "\"";
    payload += ssid;
    payload += "\"";
    payload += "}";

    sendMessage(payload);
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
}

