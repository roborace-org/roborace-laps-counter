#ifndef LAPS_COUNTER_SERVER_H
#define LAPS_COUNTER_SERVER_H

#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <Interval.h>
#include <Stopwatch.h>
#include "RaceState.h"
#include "IrReceiver.h"
#include "robots.h"
#include "LedRGB.h"

class LapsCounterServer {

public:

    LapsCounterServer(const char *ssid, const char *pass) {
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

        webSocket.begin();
        webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
            webSocketEvent(num, type, payload, length);
        });
    }

    void loop() {
        WiFiMulti.run();
        webSocket.loop();

        if (raceState == RaceState::RUNNING) {
            checkIrCodes();

            if (interval.isReady()) {
                sendRaceTime();
            }
        }
    }

private:

    RaceState raceState = RaceState::READY;

    Stopwatch raceStopwatch;
    unsigned long raceTime;

    RobotsHolder robotsHolder;

    Interval interval = Interval(10000);

    IrReceiver irReceiver;

    LedRGB ledRGB = LedRGB(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, LED_V_PIN);

    ESP8266WiFiMulti WiFiMulti;

    WebSocketsServer webSocket = WebSocketsServer(80);

    DynamicJsonBuffer jsonBuffer;


    void checkIrCodes() {
        uint32_t irCode = irReceiver.getCode();
        if (irCode > 0) {
            Serial.println(irCode, HEX);
            Robot *robot = robotsHolder.checkIrCode(irCode, raceStopwatch.time());
            if (robot) {
//                sendRobotLap(*robot);
                sendRobotsLaps(); // Broadcast all robots because place can be changed for several robots
            }
        }
    }


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
                if (raceState == RaceState::RUNNING) {
                    sendRaceTime(num);
                }
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
        } else if (root["type"] == "LAPS") {
            sendRobotsLaps(num);
        } else if (root["type"] == "LAP_MAN") {
            byte robot = root["robot"];
            int laps = root["laps"];
            robotsHolder.lapManual(robot, laps, raceStopwatch.time());
            sendRobotsLaps(num);
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

            if (raceState == RaceState::STEADY) {
                ledRGB.red();
            } else if (raceState == RaceState::RUNNING) {
                ledRGB.green();
                raceStopwatch.start();
                interval.startWithCurrentTime();
                sendRaceTime();
            } else if (raceState == RaceState::FINISH) {
                ledRGB.blue();
                raceTime = raceStopwatch.time();
                JsonObject &root = createRootObject("TIME");
                root["millis"] = raceTime;
                sendWebSocket(root);
            }
        }
    }

    void sendRobotsLaps(uint8_t num = 255) {
        for (int i = 0; i < robotsHolder.count; i++) {
            sendRobotLap(*robotsHolder.robots[i], num);
        }
    }

    void sendRobotLap(const Robot &robot, uint8_t num = 255) {
        JsonObject &root = createRootObject("LAP");
        root["num"] = robot.num;
        root["name"] = robot.name;
        root["place"] = robot.place;
        root["laps"] = robot.laps;
        root["time"] = robot.time;
        root["best"] = robot.best;
        sendWebSocket(root, num);
    }

    void sendRaceTime(uint8_t num = 255) {
        JsonObject &root = createRootObject("TIME");
        root["millis"] = raceStopwatch.time();
        sendWebSocket(root, num);
    }

    void sendWebSocket(uint8_t num, const char *type, const char *fieldName, const char *fieldValue) {
        JsonObject &resp = createRootObject(type);
        resp[fieldName] = fieldValue;
        sendWebSocket(resp, num);
    }

    void sendWebSocket(const JsonObject &root, uint8_t num = 255) {
        String json;
        root.printTo(json);
        if (num == 255) {
            webSocket.broadcastTXT(json);
        } else {
            webSocket.sendTXT(num, json);
        }
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
