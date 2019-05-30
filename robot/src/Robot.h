#ifndef LAPS_COUNTER_ROBOT_H
#define LAPS_COUNTER_ROBOT_H

#include <Timeout.h>
#include "config.h"
#include "Communication.h"
#include "RaceState.h"
#include "IrReceiver.h"
#include "LedRGB.h"


class Robot {

private:
    IrReceiver irReceiver;

    Communication communication;

    RaceState raceState = RaceState::READY;

    Timeout frameSendTimeout;
    Timeout ledTimeout;

public:

    Robot() {
        communication.init(ROBOT_SERIAL);
        communication.onRaceStateChange([&](RaceState &newRaceState) {
            updateRaceState(newRaceState);
        });
    }

    void loop() {
        communication.loop();
        checkIrReceiver();
        checkLed();
    }

private:

    void checkIrReceiver() {
        uint32_t irCode = irReceiver.getCode();
        if (irCode > 0) {
            Serial.println(irCode, HEX);
            if (raceState == RaceState::RUNNING && frameSendTimeout.isReady()) {
                Serial.println("Send Frame");
                communication.sendFrame(irCode);
                frameSendTimeout.start(FRAME_SAFE_DELAY);
                ledTimeout.start(FRAME_LED_DELAY);
                ledRGB.blue();
            }
        }
    }

    void updateRaceState(const RaceState &newRaceState) {
        if (raceState != newRaceState) {
            raceState = newRaceState;

            lightLedByRaceState();
        }
    }

    void checkLed() {
        if (ledTimeout.isReady()) {
            lightLedByRaceState();
        }
    }

    void lightLedByRaceState() const {
        switch (raceState) {
            case RaceState::READY:
                ledRGB.red();
                break;
            case RaceState::STEADY:
                ledRGB.yellow();
                break;
            case RaceState::RUNNING:
                ledRGB.green();
                break;
            case RaceState::FINISH:
                ledRGB.blue();
                break;
            case RaceState::UNKNOWN:
                ledRGB.rgb(1, 1, 1);
                break;
        }
    }

};

#endif
