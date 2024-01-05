/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _NET_COMMON_H
#define _NET_COMMON_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>

#include "net_config.h"

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class net_resolver {
public:
  net_resolver(char *buff, size_t buf_size);

  void bind_config(const char *iface_name, net_config_t *config);
  void resolve();

protected:
  char *_read_line();
  char *_read_arg(char *str);
  bool _process_line(char *line);
  char *_strip(char *line);
  bool _resolve_option(char *raw_opt);

  void _switch_interface(const char *iface);
  void _process_option(const char *option, const char *arg);

  void _process_opt_static(const char *arg);

private:
#if ESP32 || ESP_PLATFORM || ESP8266 || __APPLE__ || __linux__
  net_config_t *_wlan0;
#endif
#if ARDUINO_WT32_ETH01 || ETH_ENABLE
  net_config_t *_eth0;
#endif

  char *_buf;
  size_t _buf_size;
  size_t _offset;
  net_config_t *_curr_config;
};

#endif /* _NET_COMMON_H */
