/** See a brief introduction (right-hand button) */
#include "wifi_config.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NET_CONF_MAX_SIZE 1024 /** 1k */

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static wifi_ap_t *_wifi_config = nullptr;
static size_t _config_size = 0;
static wifi_ap_t *_wifi_handler = nullptr;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void _init_config(size_t config_count);
static void _process_option(const char *iface, const char *option,
                            const char *arg);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
int wifi_config_load(const char *str, wifi_ap_t *configs, size_t max) {
  const char *pos = str;
  size_t str_len = 0;
  bool in_block = false;
  size_t index = 0;
  while (pos != nullptr && index < max) {
    /** begin *******************************************************/
    pos = strchr(str, '\n');

    /** processing **************************************************/
    str_len = (pos == nullptr) ? strlen(str) : pos - str;
    const char *arg_pos = strchr(str, '=');
    size_t arg_len = (arg_pos == nullptr) ? 0 : pos - arg_pos - 1;
    size_t info_len = str_len - arg_len - 1;
    arg_pos += 1;

    if (false == in_block) {
      in_block = (strncmp(str, "network={", str_len) == 0);
    } else /** in_block */ {
      if (arg_len == 0 && strncmp(str, "}", 1) == 0) {
        in_block = false;
        index += 1;
      } else if (strncmp(str, "ssid", info_len) == 0) {
        configs[index].ssid = String{arg_pos + 1, arg_len - 2};
      } else if (strncmp(str, "psk", info_len) == 0) {
        configs[index].passwd = String{arg_pos + 1, arg_len - 2};
      }
    }

    /** end *********************************************************/
    str = pos + 1;
  }
  return index;
}

int wifi_config_print(char *buffer, size_t size, wifi_ap_t *configs,
                      size_t count) {

  size_t used = 0;
  uint8_t *ipv4 = nullptr;
  if (buffer == nullptr || configs == nullptr || count == 0) {
    return 0;
  }

  for (int i = 0; i < count; ++i) {
    if (configs->ssid.isEmpty()) {
      continue;
    }

    used += snprintf(buffer, size - used,
                     "network={\n"
                     "ssid=\"%s\"\n"
                     "psk=\"%s\"\n"
                     "}",
                     configs[i].ssid.c_str(), configs[i].passwd.c_str());
  }

  return used;
}