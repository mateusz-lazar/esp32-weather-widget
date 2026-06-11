# ESP32 weather widget

ESP32 based project using E-ink display controlled by GxEPD2 library to show fetched weather data from OpenWeather API.

## Features
The application displays name of the location specified by coordinates, temperature, humidity and time of the last update. The ESP32 is set to deep sleep, interrupted every 15 minutes only to fetch and display new data.

## Hardware
- ESP32 DOIT DEVKIT V1
- Waveshare 1.54 (200x200) e-Paper Module

## Software
- VS Code with PlatformIO (core v. 6.1.19)
- GxEPD2
- ArduinoJson
- HTTPClient

If using Arduino IDE, refer to migration guides.

## Setup
1. Clone repo
2. Copy config_example.h to config.h
3. Fill in config.h with your data (get OpenWeather API key at https://openweathermap.org/)
4. If using different ESP32 board, check SPI pinout (main.cpp, 13)
5. Connect ESP32 and upload