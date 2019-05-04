#ifndef ROBOT_COMMUNICATION_H
#define ROBOT_COMMUNICATION_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "RaceState.h"
#include "LedRGB.h"


typedef std::function<void(RaceState &raceState)> RaceStateCallback;

class Communication {

private:
    ESP8266WiFiMulti wiFiMulti;

    WebSocketsClient webSocket;
    bool connected = false;
    bool robotInited = false;

    DynamicJsonDocument doc = DynamicJsonDocument(1024);

    RaceStateCallback _raceStateCallback;

    int _serial = 0;

public:

    void init(int serial) {
        _serial = serial;
        wifiInit();
        webSocketInit();
    }

    void onRaceStateChange(RaceStateCallback raceStateCallback) {
        _raceStateCallback = raceStateCallback;
    }

    void loop() {
        wiFiMulti.run();
        webSocket.loop();
        if (connected && !robotInited) {
            sendRobotInit();
        }
    }

    void sendFrame(int frame) {
        createRootObject("FRAME");
        doc["frame"] = frame;
        sendWebSocket(doc);
    }

private:

    void wifiInit() {
        wiFiMulti.addAP(SSID, PASS);

        wl_status_t run;
        while ((run = wiFiMulti.run()) != WL_CONNECTED) {
            Serial.printf("[WIFI] Not connected: %d\n", run);
            blynk();
        }
        ledRGB.blue();
        const IPAddress &ip = WiFi.localIP();
        Serial.printf("[WIFI] IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
    }

    void webSocketInit() {
        webSocket.begin(SERVER_ADDRESS, WEBSOCKET_PORT);
        webSocket.onEvent([&](WStype_t type, uint8_t *payload, size_t length) {
            webSocketEvent(type, payload, length);
        });
        webSocket.setReconnectInterval(1000);
        while (!connected) {
            Serial.printf("[WSc] Not connected\n");
            webSocket.loop();
            blynk();
        }
        ledRGB.blue();
    }

    void blynk() const {
        ledRGB.blue();
        delay(100);
        ledRGB.rgb(0, 0, 0);
        delay(100);
    }

    void sendRobotInit() {
        createRootObject("ROBOT_INIT");
        sendWebSocket(doc);
        robotInited = true;
    }

    bool sendWebSocket(const DynamicJsonDocument &root) {
        String json;
        serializeJson(root, json);
        Serial.printf("[WSc] Send data: %s\n", json.c_str());
        return webSocket.sendTXT(json);
    }

    void createRootObject(const char *type) {
        doc.clear();
        doc["type"] = type;
        doc["serial"] = _serial;
    }


    void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

        switch (type) {
            case WStype_DISCONNECTED: {
                connected = false;
                robotInited = false;
                Serial.printf("[WSc] Disconnected!\n");
                break;
            }
            case WStype_CONNECTED: {
                connected = true;
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
        DeserializationError error = deserializeJson(doc, (char *) payload);
        if (error) {
            Serial.printf("[JSON] deserializeJson() failed: %s\n", error.c_str());
            return;
        }

        if (!doc.containsKey("type")) {
            return;
        }

        if (doc["type"] == "STATE") {
            processState();
        }

    }

    void processState() {
        if (!doc.containsKey("state")) {
            Serial.println("No state field");
            return;
        }

        const char *state = doc["state"];
        RaceState parsedState = parseRaceState(state);
        if (parsedState == RaceState::UNKNOWN) {
            Serial.println("UNKNOWN state");
            return;
        }

        if (_raceStateCallback) {
            _raceStateCallback(parsedState);
        }

    }

};

#endif
