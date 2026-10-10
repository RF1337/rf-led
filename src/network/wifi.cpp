// WiFi. Connect to your network and print the IP in serial.

#include <Arduino.h>
#include <WiFi.h>
#include "network/wifi.h"
#include "storage/storage.h"

enum class WifiState
{
    Connecting,
    Connected,
    ApMode
};

WifiState wifiState;
uint32_t connectStart;

void startAP(){
    WiFi.mode(WIFI_AP);
    WiFi.softAP("rf-led", "rfled123");
    Serial.println("Started AP mode. Connect to rf-led with password rfled123");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    wifiState = WifiState::ApMode;
}

void wifiBegin()
{
    String ssid, password;

    if (!loadWifiCredentials(ssid, password)) {
        Serial.println("No WiFi credentials found. Starting AP mode.");
        startAP();
        return;
    } 
    else
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        connectStart = millis();
        wifiState = WifiState::Connecting;
    }
}

void wifiLoop()
{
    if (wifiState == WifiState::Connecting)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("Connected to WiFi. IP address: ");
            Serial.println(WiFi.localIP());
            wifiState = WifiState::Connected;
        }
        else if (millis() - connectStart > 15000) // 15 seconds timeout
        {
            Serial.println("Failed to connect to WiFi. Starting AP mode.");
            startAP();
        }
    }
}