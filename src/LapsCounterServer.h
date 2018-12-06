#ifndef LAPS_COUNTER_SERVER_H
#define LAPS_COUNTER_SERVER_H

#include <DeviceWiFi.h>

class LapsCounterServer : public DeviceWiFi {

public:

    LapsCounterServer(const char *ssid, const char *pass) : DeviceWiFi(ssid, pass) {
    }

    void loop() override {
        DeviceWiFi::loop();
    }

private:

};

#endif
