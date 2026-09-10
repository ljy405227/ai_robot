
#ifndef MY_WIFI_HPP
#define MY_WIFI_HPP


#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

bool Wifi_Init(void);

void Wifi_On(void);

bool Wifi_Reconnect(void);

String callAI(String userText);

#endif  // MY_MAIN_HPP


