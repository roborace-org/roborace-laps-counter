#ifndef ROBORACE_LAPS_COUNTER_LEDRGB_H
#define ROBORACE_LAPS_COUNTER_LEDRGB_H

class LedRGB {
public:
    LedRGB(const byte redPin, const byte greenPin, const byte bluePin, const byte vPin, const bool commonAnode) :
            redPin(redPin), greenPin(greenPin), bluePin(bluePin), commonAnode(commonAnode) {

        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        pinMode(vPin, OUTPUT);

        digitalWrite(vPin, commonAnode ? HIGH : LOW);
        rgb(0, 0, 0);
    }

    void red() const { rgb(1, 0, 0); }

    void yellow() const { rgb(1, 1, 0); }

    void green() const { rgb(0, 1, 0); }

    void blue() const { rgb(0, 0, 1); }

    void rgb(byte red, byte green, byte blue) const {
        digitalWrite(redPin, commonAnode ? invert(red) : red);
        digitalWrite(greenPin, commonAnode ? invert(green) : green);
        digitalWrite(bluePin, commonAnode ? invert(blue) : blue);
    }

private:
    const byte redPin;
    const byte greenPin;
    const byte bluePin;
    const bool commonAnode;

    byte invert(byte red) const {
        return (byte) (1 - red);
    }
};

LedRGB ledRGB = LedRGB(LED_RED_PIN, LED_GREEN_PIN, LED_BLUE_PIN, LED_V_PIN, false);

#endif