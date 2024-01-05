/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IPV4_UTILS_H
#define _IPV4_UTILS_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _ipv4 ipv4_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
struct _ipv4 {
  uint8_t ipv4[4];
};

#endif /* _IPV4_UTILS_H */
