/** See a brief introduction (right-hand button) */
#include "net_config.h"
/* Private include -----------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
int net_config_load(const char *str, const char *name, net_config_t *cfg) {
  const char *pos = str;
  size_t str_len = 0;
  bool loading = false;
  while (pos != nullptr) {
    /** begin *******************************************************/
    pos = strchr(str, '\n');

    /** processing **************************************************/
    str_len = (pos == nullptr) ? strlen(str) : pos - str;
    {
      const char *arg_pos = strchr(str, ' ');
      size_t arg_len = (arg_pos == nullptr) ? 0 : pos - arg_pos - 1;
      size_t info_len = str_len - arg_len - 1;
      if (arg_len == 0) {
        continue;
      } else if (strncmp(str, "interface", info_len) == 0) {
        bool cfg_is_same = strncmp(arg_pos + 1, name, arg_len) == 0;
        if (false == loading && cfg_is_same) {
          loading = true;
        } else if (loading && false == cfg_is_same) {
          break;
        }
      } else if (strncmp(str, "static", info_len) == 0) {
        const char *name = arg_pos + 1;
        const char *data = strchr(name, '=');
        size_t name_length = (data == nullptr) ? arg_len : data - name - 1;
        size_t data_length =
            arg_len - name_length - 1 - 1 /* this 1 is for '=' */;
        data = data + 1;
        if (name_length >= arg_len - 1) {
          continue;
        } else if (strncmp(name, "ip_address", name_length) == 0) {
          uint32_t ip;
          int mask_bit;
          uint8_t *ipv4 = (uint8_t *)&ip;
          int count = sscanf(data, "%hhd.%hhd.%hhd.%hhd/%d", &ipv4[0], &ipv4[1],
                             &ipv4[2], &ipv4[3], &mask_bit);
          if (count == 5 && mask_bit >= 0 && mask_bit <= 32) {
            cfg->address = ip;
            cfg->mask_bit = mask_bit;
          }
        } else if (strncmp(name, "routers", name_length) == 0) {
          uint32_t ip;
          uint8_t *ipv4 = (uint8_t *)&ip;
          int count = sscanf(data, "%hhd.%hhd.%hhd.%hhd", &ipv4[0], &ipv4[1],
                             &ipv4[2], &ipv4[3]);
          if (count == 4) {
            cfg->gateway = ip;
          }
        } else if (strncmp(name, "domain_name_servers", name_length) == 0) {
          /** nothing todo now. */
        }
      }
    }

    /** end *********************************************************/
    str = pos + 1;
  }
  return loading;
}

int net_config_print(char *buffer, size_t size, const char *name,
                     net_config_t *cfg) {
  size_t used = 0;
  uint8_t *ipv4 = nullptr;
  if (buffer == nullptr || name == nullptr || cfg == nullptr) {
    return 0;
  }

  used += snprintf(buffer + used, size - used, "interface %s\n", name);

  ipv4 = (uint8_t *)&cfg->address;
  used +=
      snprintf(buffer + used, size - used, "static ip_address=%d.%d.%d.%d/%d\n",
               ipv4[0], ipv4[1], ipv4[2], ipv4[3], cfg->mask_bit);

  ipv4 = (uint8_t *)&cfg->gateway;
  used += snprintf(buffer + used, size - used, "static routers=%d.%d.%d.%d\n",
                   ipv4[0], ipv4[1], ipv4[2], ipv4[3]);

  return used;
}

bool is_config_valid(net_config_t cfg) {
  return cfg.address != 0 && cfg.gateway != 0 && cfg.mask_bit >= 0 &&
         cfg.mask_bit <= 32;
}

uint32_t net_config_get_mask(net_config_t cfg) {
  return 0xFFFFFFFF >> (32 - cfg.mask_bit);
}

uint8_t ip_to_mask(uint32_t mask) {
  uint32_t mask_32 = static_cast<uint32_t>(mask);
  uint8_t mask_bit_count = 0;
  for (int i = 0; i < 32; ++i) {
    if ((mask_32 & 0x80000000) == 0x80000000) {
      break;
    }
    mask_bit_count += 1;
    mask_32 <<= 1;
  }
  return 32 - mask_bit_count;
}