#ifndef LAPS_COUNTER_CONFIG_H
#define LAPS_COUNTER_CONFIG_H

#include <IPAddress.h>

// WiFi settings
char ssid[] = "ACCESS-POINT";
char pass[] = "pa$$word";

// IR
#define IR_RECV_PIN D6
#define IR_GND_PIN D7
#define IR_V_PIN D8

// Led
#define LED_RED_PIN D0
#define LED_GREEN_PIN D2
#define LED_BLUE_PIN D3
#define LED_V_PIN D1


// Server
IPAddress SERVER_ADDRESS = {192, 168, 1, 200};
#define WEBSOCKET_PORT 8888
#define MAX_ROBOTS_COUNT 10
#define SAFE_LAPS_INTERVAL 5000


// Robot
#define ROBOT_SERIAL 101
#define SAFE_FRAME_INTERVAL 1000

#endif
