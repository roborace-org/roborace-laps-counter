#ifndef LAPS_COUNTER_CONFIG_H
#define LAPS_COUNTER_CONFIG_H

#include <IPAddress.h>

// WiFi settings
char ssid[] = "ACCESS-POINT";
char pass[] = "pa$$word";

// IR
#define IR_RECV_PIN D3
#define IR_GND_PIN D2
#define IR_V_PIN D1
#define ENABLE_PIN D4

// Led
#define LED_RED_PIN D0
#define LED_V_PIN D5
#define LED_GREEN_PIN D6
#define LED_BLUE_PIN D7


// Server
IPAddress SERVER_ADDRESS = {192, 168, 1, 200};
#define WEBSOCKET_PORT 8888


// Robot
#define ROBOT_SERIAL 101
#define FRAME_SAFE_DELAY 1000
#define FRAME_LED_DELAY 500

#endif
