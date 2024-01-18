/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <WString.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef struct _wifi_config {
  String ssid;
  String passwd;
} wifi_ap_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
/**
 * @brief load a named configure from str.
 *
 * @param str config strings. content should be like UNIX dhcpcd.conf
 * @param configs config array.
 * @param size_t max configure size.
 * @return int return loaded count.
 */
int wifi_config_load(const char *str, wifi_ap_t *configs, size_t max);

int wifi_config_print(char *buffer, size_t buff_size, wifi_ap_t *configs,
                      size_t count);

int wifi_get_mac(char *buffer, size_t buff_size);

#endif /* WIFI_CONFIG_H */
