/** See a brief introduction (right-hand button) */
#include "net_common.h"
/* Private include -----------------------------------------------------------*/
#include <ctype.h>
#include <string.h>
#include <stdio.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define required_argument 1

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/

net_resolver::net_resolver(char *buff, size_t buf_size)
    :
#if ESP32 || ESP_PLATFORM || ESP8266 || __APPLE__ || __linux__
      _wlan0{nullptr},
#endif
#if ARDUINO_WT32_ETH01 || ETH_ENABLE
      _eth0{nullptr},
#endif
      _buf{buff}, _buf_size{_buf_size}, _offset{0}, _curr_config{nullptr} {
}

void net_resolver::bind_config(const char *iface_name, net_config_t *config) {
  struct {
    const char *iface;
    net_config_t *&config;
  } iface_config_map[]{
#if ESP32 || ESP_PLATFORM || ESP8266 || __APPLE__ || __linux__
      {"wlan0", _wlan0},
#endif
#if ARDUINO_WT32_ETH01 || ETH_ENABLE
      {"eth0", _eth0},
#endif
  };

  for (auto record : iface_config_map) {
    if (strcmp(iface_name, record.iface) == 0) {
      record.config = config;
      break;
    }
  }
}

void net_resolver::resolve() {
  assert(_offset == 0);
  if (_offset != 0) {
    /** resolver has resolve before. */
    return;
  }

  while (_process_line(_read_line()) == true) {
    _offset = _offset + 1;
  }
}

char *net_resolver::_read_line() {
  char *index = _buf + _offset;
  while (_buf[_offset] != '\0') {
    if (_buf[_offset] == '\n') {
      _buf[_offset] = '\0';
      break;
    }

    _offset = _offset + 1;
  }
  return index;
}

char *net_resolver::_read_arg(char *str) {
  assert(str != nullptr);

  while (isgraph(*str)) {
    str = str + 1;
  }

  char *index = nullptr;
  if (*str != '\0') {
    *str = '\0';
    index = str + 1;
  }
  return index;
}

bool net_resolver::_process_line(char *line) {
  /** process end. */
  if (line[0] == '\0') {
    return false;
  }

  _resolve_option(_strip(line));
  return true;
}

char *net_resolver::_strip(char *line) {
  char *begin = line;
  size_t end_index = strlen(line);
  assert(end_index > 0);

  while (!isgraph(begin[end_index])) {
    end_index--;
  }
  begin[end_index + 1] = '\0';

  while (!isgraph(*begin)) {
    begin++;
  }

  return begin;
}

bool net_resolver::_resolve_option(char *raw_opt) {
  char *option = raw_opt;
  char *arg = _read_arg(option);

  if (strcmp(option, "interface") == 0) {
    if (arg == nullptr) {
      /** interface should have an argument. */
    } else {
      _switch_interface(_strip(arg));
    }
  } else {
    _process_option(option, arg);
  }

  return true;
}

void net_resolver::_switch_interface(const char *iface) {
  struct {
    const char *iface;
    net_config_t *&config;
  } iface_config_map[]{
#if ESP32 || ESP_PLATFORM || ESP8266 || __APPLE__ || __linux__
      {"wlan0", _wlan0},
#endif
#if ARDUINO_WT32_ETH01 || ETH_ENABLE
      {"eth0", _eth0},
#endif
  };

  _curr_config = nullptr;
  for (auto record : iface_config_map) {
    if (strcmp(record.iface, iface) == 0) {
      _curr_config = record.config;
      break;
    }
  }
}

void net_resolver::_process_option(const char *option, const char *arg) {
  struct {
    const char *name;
    int has_arg;
    int val;
  } cf_configs[]{
      {"static", required_argument, 'S'},
  };

  int val = 0;
  for (auto record : cf_configs) {
    if (strcmp(record.name, option) == 0) {
      if (!record.has_arg || arg != nullptr) {
        val = record.val;
      }
      break;
    }
  }

  switch (val) {
  case 'S':
    _process_opt_static(arg);
    break;

  default:
    break;
  }
}

void net_resolver::_process_opt_static(const char *arg) {
  if (_curr_config == nullptr) {
    return;
  }

  if (strstr(arg, "ip_address") != nullptr) {
    int mask_bit;
    int ipv4[4];
    int count = sscanf(arg, "ip_address=%d.%d.%d.%d/%d", &ipv4[3], &ipv4[2],
                       &ipv4[1], &ipv4[0], &mask_bit);
    if (count == 5 && mask_bit >= 0 && mask_bit <= 32) {
      _curr_config->address.ipv4[0] = ipv4[0];
      _curr_config->address.ipv4[1] = ipv4[1];
      _curr_config->address.ipv4[2] = ipv4[2];
      _curr_config->address.ipv4[3] = ipv4[3];
      _curr_config->mask_bit = mask_bit;
    }
  } else if (strstr(arg, "routers") != nullptr) {
    int ipv4[4];
    int count = sscanf(arg, "routers=%d.%d.%d.%d", &ipv4[3], &ipv4[2], &ipv4[1],
                       &ipv4[0]);

    _curr_config->gateway.ipv4[0] = ipv4[0];
    _curr_config->gateway.ipv4[1] = ipv4[1];
    _curr_config->gateway.ipv4[2] = ipv4[2];
    _curr_config->gateway.ipv4[3] = ipv4[3];
  }
}
