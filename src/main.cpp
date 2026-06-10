#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../include/config.h"
#include "../include/images.h"

GxEPD2_BW <GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4)
);

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* api_key = API_KEY;
const char* lat = LAT;
const char* lon = LON;

char url[256];

const char* city;
float temperature;
int humidity;

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
void assemble_url(){
  snprintf(url, sizeof(url),
  "https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s",
  lat, lon, api_key);
  Serial.println(url);
}
void fetch_weather_data(const char* url){
  HTTPClient http; //create HTTPClient type object named http
  http.begin(url); //asign url to http where it will comunicate

  int httpValue = http.GET();
  
  if(httpValue == 200){
    WiFiClient* stream = http.getStreamPtr(); //WiFiClient belongs to HTTPClient
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *stream);

    if(!error){
      city = doc["name"];
      temperature = doc["main"]["temp"];
      humidity = doc["main"]["humidity"];
      temperature = temperature - 273.15;

      Serial.printf("City: %s\n", city);
      Serial.printf("Temperature: %f\n", temperature);
      Serial.printf("Humidity: %d\n", humidity);

    }else{
      Serial.printf("JSON error: %d\n", error);
    }
  }else{
    Serial.printf("HTTP error: %d\n", httpValue);
  }
}
void set_text_properties(){
  display.setRotation(1);
  display.setFont(&FreeSans12pt7b); //font changed in printing_process() for lines[0]
  display.setTextColor(GxEPD_BLACK);
}
void printing_process(){

//Displayed text
  const char* lines [][1] = { 
    {"City:"},
    {"Temperature:"}, 
    {"Humidity:"}
  };
  int line_count = sizeof(lines)/sizeof(lines[0]);

  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  
  do
  {
    for(int i = 0; i < line_count; i ++){
      
      if(i == 0){
        display.setFont(&FreeSansBold12pt7b);
      }

      display.setCursor(200, -10 + (i * 25));
      display.print(lines[i][0]);
      
      switch(i){
        case 0:
          display.setCursor(100, -5 + ((i+1) * 25));
          display.print(city);
          break;
        case 1:
          display.setCursor(140, -5 + ((i+1) * 25));
          display.print(temperature);
          break;
        case 2:
          display.setCursor(140, -5 + ((i+1) * 25));
          display.print(humidity);
          break;
      }

      if(i == 0){
        display.setFont(&FreeSans12pt7b);
      }
    }
    display.drawXBitmap(5, 80, icon1, 16, 16, GxEPD_BLACK);
  }while(display.nextPage());
}
void setup(){
  wifi_init();
  assemble_url();
  fetch_weather_data(url);

  display.init(115200, true, 2, false);
  set_text_properties();
  printing_process();
  display.hibernate();
}
void loop() {};