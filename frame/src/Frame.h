#include <IRremote.h>

#define CODE 0xAA00

class Frame {
public:
    Frame() {
        irsend.enableIROut(38);
    }

    void loop() {
        irsend.sendNEC(CODE, 32);
        delay(28);
    }

private:
    IRsend irsend;
};

/**
100 - 7
75 - 8
50 - 10
35 - 12
32 - 12
30 - 12
28 - 13
25 - 7
20 - 7
10 - 0
 */
