#ifndef ROBORACE_LAPS_COUNTER_ROBOTS_H
#define ROBORACE_LAPS_COUNTER_ROBOTS_H


byte nums = 0;

class Robot {
public:

    Robot(const char *name, const byte irCode) : num(++nums), place(nums), name(name), irCode(irCode) {}

    const byte num;
    const char *name;
    const byte irCode = 0x0;

    byte place = 0;
    byte laps = 0;
    unsigned long time = 0;
    unsigned long best = 0;


};

Robot robots[] = {
        Robot("Robot 1", 0x21),
        Robot("My cool robot 2", 0x22),
        Robot("The best robot 3", 0x23),
};

#endif
