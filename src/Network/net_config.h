/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UNIX_NET_CONFIG_H
#define UNIX_NET_CONFIG_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _net_config net_config_t;

struct _net_config {
  uint32_t address;
  uint32_t gateway;
  uint8_t mask_bit;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
int net_config_set(const char *iface, const net_config_t &cfg);
int net_config_clear(const char *iface);
const net_config_t *net_config_get(const char *iface);

int net_config_init(const char *f);
int net_config_update(const char *f);
int net_config_deinit();

/* Public class --------------------------------------------------------------*/
#endif /* UNIX_NET_CONFIG_H */
