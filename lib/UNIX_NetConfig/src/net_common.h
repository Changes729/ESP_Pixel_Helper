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
typedef void (*config_cb_t)(const char *iface, const char *option,
                            const char *arg);

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class net_resolver {
public:
  net_resolver(char *buff, size_t buf_size);

  void resolve(config_cb_t callback);

protected:
  char *_read_line();
  char *_read_arg(char *str);
  char *_strip(char *line);
  bool _resolve_option(config_cb_t callback, char *raw_opt);

private:
  char *_buf;
  const char *_interface;
  size_t _buf_size;
  size_t _offset;
};

#endif /* _NET_COMMON_H */
