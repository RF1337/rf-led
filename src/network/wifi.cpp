// WiFi. Connect to your network and print the IP in serial.

#include <Arduino.h>
#include <WiFi.h>
#include "network/wifi.h"
#include "storage/storage.h"
#include <DNSServer.h>
#include <ESPmDNS.h>

#define HOSTNAME "rf-led"   // reachable as http://rf-led.local

#define DNS_PORT 53
DNSServer dnsServer;

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
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
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
        WiFi.setHostname(HOSTNAME);  // name shown in the router's device list
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

            if (MDNS.begin(HOSTNAME)) {
                MDNS.addService("http", "tcp", 80);  // lets apps discover the web server
                Serial.println("mDNS: http://" HOSTNAME ".local");
            } else {
                Serial.println("mDNS failed to start");
            }

            wifiState = WifiState::Connected;
        }
        else if (millis() - connectStart > 15000) // 15 seconds timeout
        {
            Serial.println("Failed to connect to WiFi. Starting AP mode.");
            startAP();
        }
    }

    if (wifiState == WifiState::ApMode)
    {
        dnsServer.processNextRequest();
    }
}