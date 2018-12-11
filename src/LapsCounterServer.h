#ifndef LAPS_COUNTER_SERVER_H
#define LAPS_COUNTER_SERVER_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Interval.h>

class LapsCounterServer {

public:

    LapsCounterServer(const char *ssid, const char *pass) {
        WiFiMulti.addAP(ssid, pass);
        WiFiMulti.run();
        webSocket.begin();
        webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
            webSocketEvent(num, type, payload, length);
        });
    }

    void loop() {
        WiFiMulti.run();
        webSocket.loop();

        if (interval.isReady()) {
            jsonBuffer.clear();
            JsonObject &root = jsonBuffer.createObject();
            root["type"] = "PING";
            root["millis"] = millis();
            String json;
            root.printTo(json);
            webSocket.broadcastTXT(json);
        }

    }

private:

    ESP8266WiFiMulti WiFiMulti;

    Interval interval = Interval(5000);

    WebSocketsServer webSocket = WebSocketsServer(80);

    DynamicJsonBuffer jsonBuffer;


    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

        switch (type) {
            case WStype_DISCONNECTED: {
                Serial.printf("[%u] Disconnected!\n", num);
                break;
            }
            case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                jsonBuffer.clear();
                JsonObject &root = jsonBuffer.createObject();
                root["type"] = "CONNECTED";
                String json;
                root.printTo(json);
                webSocket.sendTXT(num, json);
                break;
            }
            case WStype_TEXT: {
                Serial.printf("[%u] get Text: %s\n", num, payload);
                jsonBuffer.clear();
                JsonObject &root = jsonBuffer.createObject();
                root["type"] = "MESSAGE";
                root["message"] = payload;
                String json;
                root.printTo(json);
                webSocket.sendTXT(num, json);
                break;
            }
        }

    }
};

#endif
