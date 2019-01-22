#include <Arduino.h>
#include "config.h"


#ifdef SERVER
#include "server/LapsCounterServer.h"
LapsCounterServer *device;
#endif

#ifdef ROBOT
#include "robot/Robot.h"
Robot *device;
#endif

#ifdef FRAME
#include "frame/Frame.h"
Frame *device;
#endif

void setup() {
#ifdef SERVER
    Serial.begin(115200);
    device = new LapsCounterServer(ssid, pass);
#endif
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
