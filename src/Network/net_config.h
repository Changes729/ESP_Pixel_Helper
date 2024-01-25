/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef UNIX_NET_CONFIG_H
#define UNIX_NET_CONFIG_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _net_config {
  uint32_t address;
  uint32_t gateway;
  uint8_t mask_bit;
} net_config_t;

#define NET_DEFAULT_CFG                                                        \
  net_config_t { .address = 0, .gateway = 0, .mask_bit = 0 }

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/**
 * @brief load a named configure from str.
 *
 * @param str config strings. content should be like UNIX dhcpcd.conf
 * @param name interface name.
 * @param cfg loaded configure.
 * @return int return 1 if configure load success, else return 0.
 */
int net_config_load(const char *str, const char *name, net_config_t *cfg);

/**
 * @brief print informations in a network configure.
 *
 * @param buffer buffer
 * @param size buffer size
 * @param name interface name.
 * @param cfg loaded configure.
 * @return int return print string length.
 */
int net_config_print(char *buffer, size_t size, const char *name,
                     net_config_t *cfg);

bool is_config_valid(net_config_t cfg);
uint32_t net_config_get_mask(net_config_t cfg);
uint8_t ip_to_mask(uint32_t mask);

/* Public class --------------------------------------------------------------*/
#endif /* UNIX_NET_CONFIG_H */
