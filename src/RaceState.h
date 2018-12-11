#ifndef ROBORACE_LAPS_COUNTER_RACESTATE_H
#define ROBORACE_LAPS_COUNTER_RACESTATE_H

enum class RaceState {
    READY, STEADY, RUNNING, FINISH, UNKNOWN,
};

void getRaceStateString(RaceState state, String &s) {
    switch (state) {
        case RaceState::READY:
            s = "READY";
            break;
        case RaceState::STEADY:
            s = "STEADY";
            break;
        case RaceState::RUNNING:
            s = "RUNNING";
            break;
        case RaceState::FINISH:
            s = "FINISH";
            break;
        default:
            s = "UNKNOWN";
            break;
    }

}

RaceState parseRaceState(const char *command) {
    if (strcmp("READY", command) == 0) {
        return RaceState::READY;
    } else if (strcmp("STEADY", command) == 0) {
        return RaceState::STEADY;
    } else if (strcmp("RUNNING", command) == 0) {
        return RaceState::RUNNING;
    } else if (strcmp("FINISH", command) == 0) {
        return RaceState::FINISH;
    } else {
        return RaceState::UNKNOWN;
    }
}


#endif
