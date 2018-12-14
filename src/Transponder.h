#define LAPS_COUNTER_TRANSPONDER_H
#define LAPS_COUNTER_TRANSPONDER_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "RaceState.h"
#include "IrReceiver.h"
#include "LedRGB.h"


class Transponder {

public:

    Transponder(const char *ssid, const char *pass) {
        WiFiMulti.addAP(ssid, pass);

        wl_status_t run;
        while ((run = WiFiMulti.run()) != WL_CONNECTED) {
            Serial.print(run);
            ledRGB.blue();
            delay(100);
            ledRGB.rgb(0, 0, 0);
            delay(100);
        }
        ledRGB.blue();
        const IPAddress &ip = WiFi.localIP();
        Serial.printf("[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);

        webSocket.begin("192.168.1.101", 80);
        webSocket.onEvent([&](WStype_t type, uint8_t *payload, size_t length) {
            webSocketEvent(type, payload, length);
        });
        webSocket.setReconnectInterval(1000);
    }

    void loop() {
        WiFiMulti.run();
        webSocket.loop();

        if (raceState == RaceState::RUNNING) {

        }
    }

private:

    RaceState raceState = RaceState::READY;

    LedRGB ledRGB = LedRGB(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, LED_V_PIN);

    ESP8266WiFiMulti WiFiMulti;

    WebSocketsClient webSocket;

    DynamicJsonBuffer jsonBuffer;


    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

        switch (type) {
            case WStype_DISCONNECTED: {
                Serial.printf("[WSc] Disconnected!\n");
                break;
            }
            case WStype_CONNECTED: {
                Serial.printf("[WSc] Connected to url: %s\n", payload);
                break;
            }
            case WStype_TEXT: {
                Serial.printf("[WSc] get Text: %s\n", payload);
                processInput(payload);
                break;
            }
        }

    }

    void processInput(const uint8_t *payload) {
        JsonObject &root = jsonBuffer.parseObject((char *) payload);

        if (!root.containsKey("type")) {
            return;
        }

    }

};
