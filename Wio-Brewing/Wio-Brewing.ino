#include <OneWire.h>
#include <DallasTemperature.h>

#include "TFT_eSPI.h"
#include "Free_Fonts.h"
TFT_eSPI tft;

#define pin (BCM22)

uint8_t address[8];
OneWire ow(pin);
DallasTemperature sensors(&ow);
DeviceAddress da;

void setup() {
    Serial.begin(115200);
    Serial.println("//\n// Start oneWireSearch.ino \n//");
    while(Serial.available() < 2) ; //Spin for Console
  
    tft.begin();
    tft.setRotation(3);
 
    tft.fillScreen(TFT_LIGHTGREY); // fills entire the screen with colour red
    tft.drawFastHLine(0,120,320,TFT_BLACK); //A black horizontal line starting from (0, 120)
    tft.drawFastVLine(160,0,240,TFT_BLACK);
    //tft.setTextSize(2);
    //tft.drawString("Hello world!", 0, 10);
    tft.setFreeFont(FMB12);
    tft.setCursor((320 - tft.textWidth("Connecting to Wi-Fi.."))/2, 120);
    tft.setTextColor(TFT_RED);
    tft.print("Connecting to Wi-Fi..");


    

  
  uint8_t count = 0;

  sensors.begin();

  if (ow.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(pin, DEC);
    Serial.println("[][8] = {");
    do {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7) Serial.print(", ");
      }
      Serial.println("  },");
    } while (ow.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  sensors.begin();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  if (!sensors.getAddress(da, 0)) Serial.println("Unable to find address for Device 0");
  sensors.setResolution(da, 12); //TEMPERATURE_PRECISION);
  sensors.requestTemperatures();

}
 
void loop() {
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(da), DEC);
  Serial.println();
  float tempC = sensors.getTempC(da);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.println();
  sensors.requestTemperatures();
  delay(1000);
}
