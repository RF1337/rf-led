#include "ota/ota.h"
#include <ArduinoOTA.h>

void otaBegin(){
    ArduinoOTA.setHostname("rf-led");
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.onStart([]() { Serial.println("OTA start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nOTA done"); });
    ArduinoOTA.onProgress([](unsigned int done, unsigned int total) {
        Serial.printf("OTA: %u%%\r", done * 100 / total);
    });
    ArduinoOTA.onError([](ota_error_t e) { Serial.printf("OTA error %u\n", e); });

    ArduinoOTA.begin();
}

void otaLoop(){
    ArduinoOTA.handle();
}