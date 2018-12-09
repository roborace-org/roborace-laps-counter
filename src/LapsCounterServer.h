#ifndef LAPS_COUNTER_SERVER_H
#define LAPS_COUNTER_SERVER_H

#include <DeviceWiFi.h>
#include <WebSocketsServer.h>
#include <Interval.h>

class LapsCounterServer : public DeviceWiFi {

public:

    LapsCounterServer(const char *ssid, const char *pass) : DeviceWiFi(ssid, pass) {
        webSocket.begin();
        webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
            webSocketEvent(num, type, payload, length);
        });
    }

    void loop() override {
        DeviceWiFi::loop();
        webSocket.loop();

        if (interval.isReady()) {
            webSocket.broadcastTXT("ping " + String(millis()));
        }

    }

private:

    Interval interval = Interval(5000);

    WebSocketsServer webSocket = WebSocketsServer(80);


    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

        switch (type) {
            case WStype_DISCONNECTED: {
                Serial.printf("[%u] Disconnected!\n", num);
                break;
            }
            case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                webSocket.sendTXT(num, "Connected");
                break;
            }
            case WStype_TEXT: {
                Serial.printf("[%u] get Text: %s\n", num, payload);
                webSocket.sendTXT(num, "message here");
                webSocket.broadcastTXT("message here broadcast");
                break;
            }
        }

    }
};

#endif
