#include <Arduino.h>
#include "config.h"


#ifdef ROBOT
#include "robot/Robot.h"
Robot *device;
#endif

#ifdef FRAME
#include "frame/Frame.h"
Frame *device;
#endif

void setup() {
#ifdef ROBOT
    Serial.begin(115200);
    device = new Robot(ssid, pass);
#endif
#ifdef FRAME
    device = new Frame();
#endif
}

void loop() {
    device->loop();
}
