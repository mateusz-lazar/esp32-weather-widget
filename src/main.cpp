#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "../include/config.h"

GxEPD2_BW <GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(
  GxEPD2_154_D67(/*CS=*/ 5, /*DC=*/ 17, /*RST=*/ 16, /*BUSY=*/ 4) //CHANGE SPI PINS HERE
);

char url[256];
char city[32];
float temperature;
int humidity;
char last_update[6]; //HH:MM

void wifi_init(){
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.println("Connecting");
  int attempt_counter = 0;

  while(WiFi.status() != WL_CONNECTED)
  {
    attempt_counter++;
    if(attempt_counter > 50){ //5 seconds WiFi timeout
      Serial.println("Failed to connect to WiFi");
      Serial.println("Retrying in 15 minutes");
      break;
    }
    Serial.print(".");
    delay(100);
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
  }
}
void assemble_url(){
  snprintf(url, sizeof(url),
  "https://api.openweathermap.org/data/2.5/weather?lat=%s&lon=%s&appid=%s",
  LAT, LON, API_KEY);
  Serial.println(url);
}
void fetch_weather_data(const char* url){
  HTTPClient http;
  http.begin(url);

  int httpValue = http.GET();
  
  if(httpValue == 200){
    WiFiClient* stream = http.getStreamPtr();
    
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, *stream);

    if(!error){
      strlcpy(city, doc["name"], sizeof(city));
      temperature = doc["main"]["temp"];
      temperature = temperature - 273.15;
      humidity = doc["main"]["humidity"];

      time_t dt = doc["dt"]; // get Unix timestamp
      int tz = doc["timezone"]; //time zone offset
      time_t local_time = dt + tz; //calculate local time
      int hours = (local_time % 86400) / 3600; // 60s * 60m * 24h = 86 400, %86400 local time to get number of seconds elapsed today TOTAL, divided by 3600 (60s * 60m) to get # of hours
      int minutes = (local_time % 3600) / 60; // %3600 to get # of seconds elapsed today but without full hours
      
      snprintf(last_update, sizeof(last_update),
      "%02d:%02d", hours, minutes);

      Serial.printf("City: %s\n", city);
      Serial.printf("Temperature: %f\n", temperature);
      Serial.printf("Humidity: %d\n", humidity);
      Serial.printf("Last update: %s\n", last_update);

    }else{
      Serial.printf("JSON error: %s\n", error.c_str());
    }
  }else{
    Serial.printf("HTTP error: %d\n", httpValue);
  }
  http.end();
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
    {"Humidity:"},
    {"Last update:"}
  };
  int line_count = sizeof(lines)/sizeof(lines[0]);

  display.setFullWindow();
  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  
  do
  {
    if(WiFi.status() == WL_CONNECTED){  //display weather data if WiFi connected
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
          case 3:
            display.setCursor(140, -5 + ((i+1) * 25));
            display.print(last_update);
            break;
        }
  
        if(i == 0){
          display.setFont(&FreeSans12pt7b);
        }
      }
    }else{  //display WiFi error message
        display.setCursor(200, -10);
        display.print("WiFi error");
        display.setCursor(200, 15);
        display.print("Retrying in 15m");
    }
  }while(display.nextPage());
}
void setup(){
  wifi_init();
  
  if(WiFi.status() == WL_CONNECTED){
    assemble_url();
    fetch_weather_data(url);
  }
  display.init(115200, true, 2, false);
  set_text_properties();
  printing_process();

  display.hibernate();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
void loop() {};