/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ENCODER_H
#define ENCODER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <Arduino.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef void (*encoder_callback_t)(long);

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
int encoder_init(uint8_t int_a, uint8_t int_b);
long get_encoder_count(bool clean_up);

#endif //! ENCODER_H