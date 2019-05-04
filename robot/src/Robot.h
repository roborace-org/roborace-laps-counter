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
    }

private:

    void checkIrReceiver() {
        uint32_t irCode = irReceiver.getCode();
        if (irCode > 0) {
            Serial.println(irCode, HEX);
            if (raceState == RaceState::RUNNING && frameSendTimeout.isReady()) {
                Serial.println("Send Frame");
                communication.sendFrame(irCode);
                frameSendTimeout.start(SAFE_FRAME_INTERVAL);
            }
        }
    }

    void updateRaceState(const RaceState &newRaceState) {
        if (raceState != newRaceState) {
            raceState = newRaceState;

            if (raceState == RaceState::READY) {
                ledRGB.red();
            } else if (raceState == RaceState::STEADY) {
                ledRGB.yellow();
            } else if (raceState == RaceState::RUNNING) {
                ledRGB.green();
            } else if (raceState == RaceState::FINISH) {
                ledRGB.blue();
            }
        }
    }

};

#endif
