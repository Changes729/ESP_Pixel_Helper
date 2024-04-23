/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TOUCH_H
#define TOUCH_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <Adafruit_MPR121.h>
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
namespace Touch {

/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
void touch_init();

void set_thresholds(uint8_t touch, uint8_t release);
uint16_t check_touch_state();
bool is_touch_started();

void set_debug_info(char *buffer, size_t size);
};     // namespace Touch
#endif /* TOUCH_H */
