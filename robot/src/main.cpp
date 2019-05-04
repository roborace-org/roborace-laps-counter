#include <Arduino.h>
#include "config.h"
#include "Robot.h"

Robot *device;

void setup() {
    Serial.begin(115200);
    device = new Robot(ssid, pass);
}

void loop() {
    device->loop();
}
