/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WIFI_COMMON_H
#define WIFI_COMMON_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stddef.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
typedef void (*config_cb_t)(const char *settings, const char *option,
                            const char *arg);

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class wifi_resolver {
public:
  static size_t count_settings(const char *config_raw);

public:
  wifi_resolver(char *buff, size_t buf_size);

  void resolve(config_cb_t callback);

public:
  char *_read_line();
  char *_read_arg(char *str);
  char *_strip(char *line);
  bool _resolve_settings(config_cb_t callback, char *raw_opt);

private:
  char *_buf;
  const char *_settings;
  size_t _buf_size;
  size_t _offset;
};

#endif /* WIFI_COMMON_H */
