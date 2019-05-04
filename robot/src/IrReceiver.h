#ifndef ROBORACE_LAPS_COUNTER_IRRECEIVER_H
#define ROBORACE_LAPS_COUNTER_IRRECEIVER_H

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

class IrReceiver {
public:

    IrReceiver() {
        pinMode(IR_GND_PIN, OUTPUT);
        pinMode(IR_V_PIN, OUTPUT);
        digitalWrite(IR_GND_PIN, LOW);
        digitalWrite(IR_V_PIN, HIGH);

        irrecv.enableIRIn();
    }

    uint32_t getCode() {
        if (irrecv.decode(&results)) {
            uint64_t value = results.value;
            irrecv.resume();

            if (value <= 0xFFFF) {
                return (uint32_t) value;
            }
        }
        return 0x0;
    }


private:

    IRrecv irrecv = IRrecv(IR_RECV_PIN);
    decode_results results;

};

#endif
