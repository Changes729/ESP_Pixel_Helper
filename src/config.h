/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CONFIG_H
#define _CONFIG_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "IPAddress.h"
#include <string.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define NUM_LEDS 3
constexpr auto DATA_PIN = 5;

#define SERIAL_BAUDRATE 19200
#define ENCODER_PIN_A 39
#define ENCODER_PIN_B 36
#define ENCODER_PULSE 360
#define ENCODER_EDGE (ENCODER_PULSE * 4)
#define ENCONTER_TO_ANGLE(encoder_counter_num)                                 \
  ((encoder_counter_num) / (ENCODER_EDGE / 4))

constexpr auto PN5180_RST = 32;
constexpr auto PN5180_NSS = 33;
constexpr auto PN5180_BUSY = 35;
constexpr auto PN5180_SCK = 12;
constexpr auto PN5180_MISO = 14;
constexpr auto PN5180_MOSI = 15;

constexpr auto MPR121_SDA = 2;
constexpr auto MPR121_SCL = 4;

constexpr auto TOUCH_LEFT = 0;
constexpr auto TOUCH_RIGHT = 11;

constexpr auto ENCODER_WATCH_UP = 500 /* ms */;

/* Public typedef ------------------------------------------------------------*/
struct BoxRingScreenCFG {
  IPAddress ip;
  int port;

  String ip_str; /** temp */
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
BoxRingScreenCFG init_rs_cfg();
BoxRingScreenCFG rs_cfg();
void update_rs_cfg(IPAddress ip, int port);

#endif /* _CONFIG_H */
