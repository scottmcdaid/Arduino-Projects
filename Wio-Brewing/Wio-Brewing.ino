/* M14 Pinout
 *  1 = Pos
 *  2 = Neg
 *  3 = Signal (4k7 PullUp)
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#include "TFT_eSPI.h"
#include "Free_Fonts.h"
#include"seeed_line_chart.h"
TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);

#define pin (BCM22)

typedef struct sensor {
  DeviceAddress addr;
  float curr_tempC;
} sensor_t;

uint8_t address[8];
OneWire ow(pin);
DallasTemperature sensors(&ow);
sensor_t sensor_mashtun = { {0x28, 0x58, 0xB7, 0x79, 0x0B, 0x00, 0x00, 0x75 }, 0};
sensor_t sensor_rims = { {0x28, 0x68, 0x93, 0x79, 0x0B, 0x00, 0x00, 0x7F }, 0};

#define GRAPH_MAX_SAMPLES (120)
doubles data_mashtun;

void tft_render_sensor(uint8_t loc_x, uint8_t loc_y, char *name, sensor_t *sensor){
  tft.setFreeFont(FMB12);
  tft.setTextDatum(TC_DATUM);
  //tft.setCursor((loc_x * 160)+(160 - tft.textWidth(name))/2, loc_y*120 + tft.fontHeight() + 1);
  tft.setTextColor(TFT_BLUE, TFT_LIGHTGREY);
  tft.drawString(name, (loc_x * 160) + 80, loc_y*120 + 1 );
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.setFreeFont(FMB18);
  tft.setTextDatum(CC_DATUM);
  tft.setTextPadding(118);
  tft.drawFloat(sensor->curr_tempC, 2, (loc_x * 160) + 80, loc_y*120 + 60);
}

void tft_render_sensors(){
  tft.drawFastHLine(0,120,320,TFT_BLACK); //A black horizontal line starting from (0, 120)
  tft.drawFastVLine(160,0,240,TFT_BLACK);
  tft_render_sensor(0,0, "MashTun", &sensor_mashtun);
  tft_render_sensor(1,0, "RIMS", &sensor_rims);
}

void tft_render_graph(){
  spr.fillSprite(TFT_WHITE);
  auto content = line_chart(2, 0); //(x,y) where the line graph begins
       content
              .height(tft.height()) //actual height of the line chart
              .width(tft.width() - content.x() * 2) //actual width of the line chart
              .based_on(15.0) //Starting point of y-axis, must be a float
              .show_circle(false) //drawing a cirle at each point, default is on.
              .value(data_mashtun) //passing through the data to line graph
              .color(TFT_PURPLE) //Setting the color for the line
              .draw();
 
  spr.pushSprite(0, 0);
}

void setup() {
  //Serial Debug
  Serial.begin(115200);
  Serial.println("//\n// Start Wio-Brewing.ino \n//");
  //while(Serial.available() < 2) ; //Spin for Console Wio clumbsyly reconnects

  //Init LCD and Physical rotation
  tft.begin();
  tft.setRotation(3);
  spr.createSprite(TFT_HEIGHT,TFT_WIDTH); //Graph Sprite
  tft.fillScreen(TFT_LIGHTGREY);

  tft_render_sensors();
  
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
  //if (!sensors.getAddress(sensor_mashtun.addr, 0)) Serial.println("Unable to find address for Device 0");
  sensors.setResolution(sensor_mashtun.addr, 12); //TEMPERATURE_PRECISION);
  sensors.setResolution(sensor_rims.addr, 12); //TEMPERATURE_PRECISION);

  sensors.requestTemperatures();

}

void sensor_update_temp(sensor_t *s){
  float tempC;
  tempC = sensors.getTempC(s->addr);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    s->curr_tempC = -99.9;
    Serial.println("Error: Could not read temperature data");
    return;
  } else
  {
    s->curr_tempC = tempC;
    Serial.print("Temp C: ");
    Serial.print(tempC);
    Serial.println();
  }
  return;
}
 
void loop() {
  sensor_update_temp(&sensor_mashtun);
  sensor_update_temp(&sensor_rims);
  
  if (data_mashtun.size() == GRAPH_MAX_SAMPLES) {
    data_mashtun.pop();//this is used to remove the first read variable
  }
  data_mashtun.push(sensor_mashtun.curr_tempC);
  tft_render_sensors();
  //tft_render_graph();
  sensors.requestTemperatures();
  delay(1000);
}
