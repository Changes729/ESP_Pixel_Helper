/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NFC_H
#define _NFC_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <Arduino.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
void nfc_init();

void update_nfc();

bool has_uid(uint8_t *buffer = nullptr, size_t size = 0);
bool has_uid(String &out, const char seq = '\0');

#endif /* _NFC_H */
