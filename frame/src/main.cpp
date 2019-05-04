#include "Frame.h"
Frame *device;

void setup() {
    device = new Frame();
}

void loop() {
    device->loop();
}
