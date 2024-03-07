/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TOUCH_H
#define TOUCH_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
namespace Touch {

/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
void touch_init();

uint16_t check_touch_state();
bool is_touch_started();

};     // namespace Touch
#endif /* TOUCH_H */
