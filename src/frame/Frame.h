#include <IRremote.h>

#define CODE 0xAA00

class Frame {
public:
    Frame() {
        irsend.enableIROut(38);
    }

    void loop() {
        irsend.sendNEC(CODE, 32);
        delay(10);
    }

private:
    IRsend irsend;
};
