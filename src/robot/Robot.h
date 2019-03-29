#ifndef LAPS_COUNTER_ROBOT_H
#define LAPS_COUNTER_ROBOT_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <Timeout.h>
#include "RaceState.h"
#include "IrReceiver.h"
#include "LedRGB.h"


class Robot {

private:
    IrReceiver irReceiver;

    RaceState raceState = RaceState::READY;

    LedRGB ledRGB = LedRGB(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, LED_V_PIN);

    ESP8266WiFiMulti wiFiMulti;

    WebSocketsClient webSocket;

    DynamicJsonDocument doc = DynamicJsonDocument(1024);

    Timeout frameSendTimeout;

public:

    Robot(const char *ssid, const char *pass) {
        wifiInit(ssid, pass);

        webSocketInit();

        robotInit();
    }

    void loop() {
        wiFiMulti.run();
        webSocket.loop();

        if (raceState == RaceState::RUNNING) {
            checkIrReceiver();
        }
    }

private:

    void wifiInit(const char *ssid, const char *pass) {
        wiFiMulti.addAP(ssid, pass);

        wl_status_t run;
        while ((run = wiFiMulti.run()) != WL_CONNECTED) {
            Serial.print(run);
            ledRGB.blue();
            delay(100);
            ledRGB.rgb(0, 0, 0);
            delay(100);
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
        webSocket.loop();
    }

    void robotInit() {

        createRootObject("ROBOT_INIT");
        while (!sendWebSocket(doc)) {
            delay(200);
            webSocket.loop();
        };
    }

    void checkIrReceiver() {
        uint32_t irCode = irReceiver.getCode();
        if (irCode > 0 && frameSendTimeout.isReady()) {
            Serial.println(irCode, HEX);
            createRootObject("FRAME");
            doc["frame"] = irCode;
            sendWebSocket(doc);
            frameSendTimeout.start(SAFE_FRAME_INTERVAL);
        }
    }


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
        DeserializationError error = deserializeJson(doc, (char *) payload);
        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
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
            return;
        }

        const char *state = doc["state"];
        RaceState parsedState = parseRaceState(state);
        if (parsedState == RaceState::UNKNOWN) {
            return;
        }

        if (raceState != parsedState) {
            raceState = parsedState;

            if (raceState == RaceState::STEADY) {
                ledRGB.red();
            } else if (raceState == RaceState::RUNNING) {
                ledRGB.green();
            } else if (raceState == RaceState::FINISH) {
                ledRGB.blue();
            }
        }

    }

    bool sendWebSocket(const DynamicJsonDocument &root) {
        String json;
        serializeJson(root, json);
        Serial.println(json);
        return webSocket.sendTXT(json);
    }

    void createRootObject(const char *type) {
        doc.clear();
        doc["type"] = type;
        doc["serial"] = ROBOT_SERIAL;
    }

};

#endif
