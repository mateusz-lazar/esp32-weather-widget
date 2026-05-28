#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

#include <WiFi.h>
#include "../include/config.h"
#include "../include/images.h"

GxEPD2_BW <GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)
);

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

//Displayed text
const char* lines [][1] = { 
  {"Temperature:"}, 
  {"Humidity:"}
  };
int line_count = sizeof(lines)/sizeof(lines[0]);
  
void wifi_init(){
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}
void printing_process(){
  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  do
  {
    for(int i = 0; i < line_count; i ++)
    {
      if(i == 0)
      {
        display.setFont(&FreeSansBold12pt7b);
      }
      display.setCursor(200, -10 + (i * 25));
      display.print(lines[i][0]);
      if(i == 0)
      {
        display.setFont(&FreeSans12pt7b);
      }
    }
    display.drawXBitmap(5, 80, icon1, 16, 16, GxEPD_BLACK);
  }while(display.nextPage());
}
void set_text_properties(){
  display.setRotation(1);
  display.setFont(&FreeSans12pt7b); //font changed in printing_process() for lines[0]
  display.setTextColor(GxEPD_BLACK);
}
void setup(){
  wifi_init();
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  set_text_properties();
  printing_process();
  display.hibernate();
}
void loop() {};