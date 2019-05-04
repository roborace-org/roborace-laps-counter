#include <Arduino.h>
#include "Robot.h"

Robot *device;

void setup() {
    Serial.begin(115200);
    device = new Robot();
}

void loop() {
    device->loop();
}
