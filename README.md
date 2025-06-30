# Connect a BME280 to a LilyGO TTGO T3 LoRa32 433MHz V1.6.1 ESP32

The ESP will read the data from the sensor and puts it over the air.
The data is received by another TTGO LoRa32 using [OpenMQTTGateway](https://docs.openmqttgateway.com/)

## Layout TTGO LoRa32 V2.1 T3 V1.6

![image](https://github.com/umbm/TTGO-LoRa32-V2.1-T3_V1.6/blob/master/T3_1.6.jpg)

![image](https://www.tinytronics.nl/image/cache/catalog/products_2023/bme280-digital-barometer-pressure-and-humidity-sensor-module-back-1500x1500.jpg)

## Where to buy

[LoRa32](https://www.tinytronics.nl/nl/development-boards/microcontroller-boards/met-lora/lilygo-ttgo-t3-lora32-433mhz-v1.6.1-esp32)
[BME280](https://www.tinytronics.nl/nl/sensoren/lucht/druk/bme280-digitale-barometer-druk-en-vochtigheid-sensor-module)

## How to connect BME280 to ESP32
BME280 | ESP
------------ | -------------
VCC | 3.3V
GND | GND
SCL | GPIO22
SDA | GPIO21

![image](https://github.com/resimons/esp32-lilygo-bme280/blob/main/images/IMG_7154.jpg)
