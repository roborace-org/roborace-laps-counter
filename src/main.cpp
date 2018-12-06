#include <Arduino.h>
#include "config.h"
#include "LapsCounterServer.h"

LapsCounterServer *device;

void setup() {
    Serial.begin(115200);
    device = new LapsCounterServer(ssid, pass);
}

void loop() {
    device->loop();
}
