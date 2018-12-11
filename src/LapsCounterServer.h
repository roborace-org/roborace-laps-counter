#ifndef LAPS_COUNTER_SERVER_H
#define LAPS_COUNTER_SERVER_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Interval.h>
#include <Stopwatch.h>
#include "RaceState.h"

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

        if (interval.isReady() && raceState == RaceState::RUNNING) {
            JsonObject &root = createRootObject("TIME");
            root["millis"] = raceStopwatch.time();
            sendWebSocket(root);
        }

    }

private:

    RaceState raceState = RaceState::READY;

    Stopwatch raceStopwatch;
    unsigned long raceTime;

    Interval interval = Interval(10000);

    ESP8266WiFiMulti WiFiMulti;

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

                String raceStateString;
                getRaceStateString(raceState, raceStateString);
                sendWebSocket(num, "STATE", "state", raceStateString.c_str());
                break;
            }
            case WStype_TEXT: {
                Serial.printf("[%u] get Text: %s\n", num, payload);
                processInput(num, payload);
                break;
            }
        }

    }

    void processInput(uint8_t num, const uint8_t *payload) {
        JsonObject &root = jsonBuffer.parseObject((char *) payload);

        if (!root.containsKey("type")) {
            return sendWebSocket(num, "ERROR", "error", "Missing field 'type'");
        }

        if (root["type"] == "COMMAND") {
            processCommand(num, root);
        } else if (root["type"] == "STATE") {
            String raceStateString;
            getRaceStateString(raceState, raceStateString);
            sendWebSocket(num, "STATE", "state", raceStateString.c_str());
        } else {
            String s = String("Unknown type: ");
            s.concat((const char *) root["type"]);
            sendWebSocket(num, "ERROR", "error", s.c_str());
        }
    }

    void processCommand(uint8_t num, const JsonObject &root) {

        if (!root.containsKey("state")) {
            return sendWebSocket(num, "ERROR", "error", "Missing field 'state'");
        }

        const char *state = root["state"];
        RaceState parsedState = parseRaceState(state);
        if (parsedState == RaceState::UNKNOWN) {
            String s = "Unknown state: ";
            s.concat(state);
            return sendWebSocket(num, "ERROR", "error", s.c_str());
        }
        changeStateAndBroadcastState(parsedState);

    }

    void changeStateAndBroadcastState(const RaceState &parsedState) {
        if (raceState != parsedState) {
            raceState = parsedState;

            JsonObject &broadcast = createRootObject("STATE");
            String raceStateString;
            getRaceStateString(raceState, raceStateString);
            broadcast["state"] = raceStateString;
            sendWebSocket(broadcast);

            if (raceState == RaceState::RUNNING) {
                raceStopwatch.start();
                interval.startWithCurrentTime();
                JsonObject &root = createRootObject("TIME");
                root["millis"] = raceStopwatch.time();
                sendWebSocket(root);
            } else if (raceState == RaceState::FINISH) {
                raceTime = raceStopwatch.time();
                JsonObject &root = createRootObject("TIME");
                root["millis"] = raceTime;
                sendWebSocket(root);
            }
        }
    }

    void sendWebSocket(uint8_t num, const char *type, const char *fieldName, const char *fieldValue) {
        JsonObject &resp = createRootObject(type);
        resp[fieldName] = fieldValue;
        sendWebSocket(resp, num);
    }

    void sendWebSocket(const JsonObject &root, uint8_t num) {
        String json;
        root.printTo(json);
        webSocket.sendTXT(num, json);
    }

    void sendWebSocket(const JsonObject &root) {
        String json;
        root.printTo(json);
        webSocket.broadcastTXT(json);
    }

    JsonObject &createRootObject(const char *type) {
        JsonObject &resp = createRootObject();
        resp["type"] = type;
        return resp;
    }

    JsonObject &createRootObject() {
        jsonBuffer.clear();
        return jsonBuffer.createObject();
    }
};

#endif
