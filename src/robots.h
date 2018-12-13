#ifndef ROBORACE_LAPS_COUNTER_ROBOTS_H
#define ROBORACE_LAPS_COUNTER_ROBOTS_H


byte nums = 0;

class Robot {
public:

    Robot(const char *name, const uint32_t irCode) : num(++nums), place(nums), name(name), irCode(irCode) {}

    const byte num;
    const char *name;
    const uint32_t irCode = 0x0;

    byte place;
    byte laps = 0;
    unsigned long time = 0;
    unsigned long best = 0;


};

class RobotsHolder {
public:

    RobotsHolder() {
        addRobot("Robot 1", 0xAAAA);
        addRobot("My cool robot 2", 0xABCD);
        addRobot("The best robot 3", 0xBEAF);
    }

    void addRobot(const char *name, const uint32_t irCode) {
        robots[count++] = new Robot(name, irCode);
    }

    Robot *robots[10];

    byte count = 0;

    Robot *checkIrCode(uint32_t irCode, unsigned long raceTime) {
        Robot *robot = findRobotByIrCode(irCode);
        if (robot && raceTime - robot->time >= SAFE_LAPS_INTERVAL) {
            robot->laps++;
            robot->time = raceTime;
            recalculatePlaces(robot);
            return robot;
        }
        return NULL;
    }

    Robot *findRobotByIrCode(uint32_t irCode) const {
        for (int i = 0; i < count; i++) {
            if (robots[i]->irCode == irCode) {
                return robots[i];
            }
        }
        return NULL;
    }

private:

    void recalculatePlaces(Robot *robot) {
        for (int i = 0; i < count; i++) {
            if (robot->place == robots[i]->place + 1) {
                if (robot->laps > robots[i]->laps) {
                    robot->place--;
                    robots[i]->place++;
                    recalculatePlaces(robot);
                }
                break;
            }
        }
    }


};

#endif
