/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <IPAddress.h>
#include <stdbool.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
bool debug_enable();
void set_debug(bool enable);

IPAddress debug_ip();
uint32_t debug_port();
void update_debug_client(IPAddress address, uint32_t port = 3333);

/* Public class --------------------------------------------------------------*/
#endif /* MAIN_H */
