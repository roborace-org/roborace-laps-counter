#ifndef ROBORACE_LAPS_COUNTER_LEDRGB_H
#define ROBORACE_LAPS_COUNTER_LEDRGB_H

class LedRGB {
public:
    LedRGB(const byte redPin, const byte greenPin, const byte bluePin, const byte vPin) :
            redPin(redPin), greenPin(greenPin), bluePin(bluePin) {
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        pinMode(vPin, OUTPUT);

        digitalWrite(vPin, HIGH);
        rgb(0, 0, 0);
    }

    void red() const { rgb(1, 0, 0); }

    void yellow() const { rgb(1, 1, 0); }

    void green() const { rgb(0, 1, 0); }

    void blue() const { rgb(0, 0, 1); }

    void rgb(byte red, byte green, byte blue) const {
        digitalWrite(redPin, invert(red));
        digitalWrite(greenPin, invert(green));
        digitalWrite(bluePin, invert(blue));
    }

    byte invert(byte red) const {
        return (byte) (1 - red);
    }

private:
    const byte redPin;
    const byte greenPin;
    const byte bluePin;
};

#endif