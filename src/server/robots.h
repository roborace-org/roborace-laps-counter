#ifndef ROBORACE_LAPS_COUNTER_ROBOTS_H
#define ROBORACE_LAPS_COUNTER_ROBOTS_H


byte nums = 0;

class Robot {
public:

    Robot(const char *name, const byte serial) : num(++nums), place(nums), name(name), serial(serial) {}

    const byte num;
    const char *name;
    const byte serial = 0;

    byte place;
    byte laps = 0;
    unsigned long time = 0;
    unsigned long best = 0;


};

class RobotsHolder {
public:

    Robot *addRobot(const char *name, const byte serial) {
        Robot *robot = findRobotBySerial(serial);
        if (robot == NULL && count < MAX_ROBOTS_COUNT) {
            robot = new Robot(name, serial);
            robots[count++] = robot;
        }

        return robot;
    }

    Robot *robots[MAX_ROBOTS_COUNT];

    byte count = 0;

    Robot *checkIrCode(uint32_t irCode, unsigned long raceTime) {
        Robot *robot = findRobotBySerial(irCode);
        if (robot && raceTime - robot->time >= SAFE_LAPS_INTERVAL) {
            robot->laps++;
            robot->time = raceTime;
            recalculatePlaces(robot);
            return robot;
        }
        return NULL;
    }

    Robot *findRobotBySerial(byte code) const {
        for (int i = 0; i < count; i++) {
            if (robots[i]->serial == code) {
                return robots[i];
            }
        }
        return NULL;
    }

    void lapManual(byte num, int laps, unsigned long raceTime) {
        for (int i = 0; i < count; i++) {
            if (robots[i]->num == num) {
                robots[i]->laps += laps;
                if (laps > 0) {
                    robots[i]->time = raceTime;
                }
                recalculatePlaces(robots[i]);
            }
        }
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
