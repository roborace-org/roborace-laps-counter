#include <Arduino.h>
#include "config.h"


#ifdef RECEIVER

#include "LapsCounterServer.h"

LapsCounterServer *device;

#else

#include "Transponder.h"

Transponder *device;

#endif

void setup() {
    Serial.begin(115200);
#ifdef RECEIVER
    device = new LapsCounterServer(ssid, pass);
#else
    device = new Transponder(ssid, pass);
#endif

}

void loop() {
    device->loop();
}
