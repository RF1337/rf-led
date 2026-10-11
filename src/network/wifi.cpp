// WiFi. Connect to your network and print the IP in serial.

#include <Arduino.h>
#include <WiFi.h>
#include "network/wifi.h"
#include "storage/storage.h"
#include <DNSServer.h>
#include <ESPmDNS.h>
#include "ota/ota.h"

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

String savedSsid, savedPassword;

uint32_t lastRetry;
bool retrying = false;

void startAP(){
    WiFi.disconnect();
    WiFi.mode(WIFI_AP_STA);
    lastRetry = millis();
    WiFi.softAP("rf-led", "rfled123");
    dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
    Serial.println("Started AP mode. Connect to rf-led with password rfled123");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    wifiState = WifiState::ApMode;
}

void onConnected(){
            Serial.print("Connected to WiFi. IP address: ");
            Serial.println(WiFi.localIP());

            if (MDNS.begin(HOSTNAME)) {
                MDNS.addService("http", "tcp", 80);  // lets apps discover the web server
                Serial.println("mDNS: http://" HOSTNAME ".local");
            } else {
                Serial.println("mDNS failed to start");
            }

            wifiState = WifiState::Connected;
            otaBegin();  // Start OTA
}

void wifiBegin()
{
    if (!loadWifiCredentials(savedSsid, savedPassword)) {
        Serial.println("No WiFi credentials found. Starting AP mode.");
        startAP();
        return;
    } 
    else
    {
        WiFi.setHostname(HOSTNAME);  // name shown in the router's device list
        WiFi.mode(WIFI_STA);
        WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
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
            onConnected();
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
        if (WiFi.status() == WL_CONNECTED)
        {
            dnsServer.stop();
            WiFi.softAPdisconnect(true);
            WiFi.mode(WIFI_STA);
            retrying = false;
            onConnected();
        }
        else if (retrying && millis() - lastRetry > 10000)
        {
            WiFi.disconnect();
            retrying = false;
        }
        else if (!retrying && savedSsid.length() > 0 && WiFi.softAPgetStationNum() == 0 && millis() - lastRetry > 60000) // if no clients connected, try to connect to saved WiFi
        {
            Serial.println("Retrying to connect to saved WiFi: ");
            WiFi.begin(savedSsid.c_str(), savedPassword.c_str());
            retrying = true;
            lastRetry = millis();
        }
    }
}