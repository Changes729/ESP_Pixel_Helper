/** See a brief introduction (right-hand button) */
#include "net_config.h"
/* Private include -----------------------------------------------------------*/
#if ESP32 || ESP8266
#include <LittleFS.h>
#endif

#include "net_common.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define required_argument 1
#define NET_CONF_MAX_SIZE 1024 /** 1k */
#define NET_DEFAULT_CFG                                                        \
  net_config_t { .address = {0, 0, 0, 0}, .gateway{0, 0, 0, 0}, .mask_bit = 32 }

/* Private typedef -----------------------------------------------------------*/
typedef struct _net_interface _net_iface_t;

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static _net_iface_t *_iface_cfg = nullptr;
static size_t _iface_size = 0;

/* Private class -------------------------------------------------------------*/
struct _net_interface {
  const char *iface;
  net_config_t conf;
};

/* Private function prototypes -----------------------------------------------*/
static net_config_t *_net_config_get(const char *iface);
static void _init_iface();
static bool _is_default_config(const net_config_t &cfg);
static void _process_option(const char *iface, const char *option,
                            const char *arg);
static void _process_opt_static(net_config_t &conf, const char *arg);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
int net_config_set(const char *iface, const net_config_t &cfg) {
  net_config_t *conf = _net_config_get(iface);
  if (conf != nullptr) {
    memcpy(conf, &cfg, sizeof(net_config_t));
  }
  return 0;
}

int net_config_clear(const char *iface) {
  return net_config_set(iface, NET_DEFAULT_CFG);
}

const net_config_t *net_config_get(const char *iface) {
  return _net_config_get(iface);
}

int net_config_init(const char *f) {
  int ret = 0;

  do {
    _init_iface();

#if __APPLE__ || __linux__
    /** read file */
    char buf[NET_CONF_MAX_SIZE]{"interface wlan0\n"
                                "static ip_address=192.168.1.23/24\n"
                                "static routers=192.168.1.1\n"
                                "static domain_name_servers=192.168.1.1\n"};
    net_resolver{buf, strlen(buf)}.resolve(_process_option);

#else
    /** file should exists. */
    if (!LittleFS.exists(f)) {
      ret = -1;
      break;
    }

    /** open file */
    File file = LittleFS.open(f, "r");
    {
      /** read file */
      char buf[NET_CONF_MAX_SIZE]{0};
      size_t buf_size = file.readBytes(buf, sizeof(buf));
      net_resolver{buf, buf_size}.resolve(_process_option);
    }
    file.close();
#endif
  } while (0);

  return ret;
}

int net_config_update(const char *f) {
  assert(_iface_cfg != nullptr && f != nullptr);
  if (_iface_cfg == nullptr || f == nullptr) {
    return 0;
  }

  File config_file = LittleFS.open(f, "w");
  for (int i = 0; i < _iface_size; ++i) {
    if (_is_default_config(_iface_cfg[i].conf)) {
      continue;
    }

    config_file.print("interface ");
    config_file.println(_iface_cfg[i].iface);

    config_file.print("static ");
    config_file.print("ip_address=");
    config_file.print(_iface_cfg[i].conf.address.ipv4[3]);
    config_file.print('.');
    config_file.print(_iface_cfg[i].conf.address.ipv4[2]);
    config_file.print('.');
    config_file.print(_iface_cfg[i].conf.address.ipv4[1]);
    config_file.print('.');
    config_file.print(_iface_cfg[i].conf.address.ipv4[0]);
    config_file.print('/');
    config_file.println(_iface_cfg[i].conf.mask_bit);

    config_file.print("static ");
    config_file.print("routers=");
    config_file.print(_iface_cfg[i].conf.gateway.ipv4[3]);
    config_file.print('.');
    config_file.print(_iface_cfg[i].conf.gateway.ipv4[2]);
    config_file.print('.');
    config_file.print(_iface_cfg[i].conf.gateway.ipv4[1]);
    config_file.print('.');
    config_file.println(_iface_cfg[i].conf.gateway.ipv4[0]);
  }
  config_file.close();

  return 0;
}

int net_config_deinit() {
  delete[] _iface_cfg;
  _iface_cfg = nullptr;
  return 0;
}

static net_config_t *_net_config_get(const char *iface) {
  assert(iface != nullptr);

  net_config_t *conf = nullptr;
  if (_iface_cfg != nullptr) {
    for (int i = 0; i < _iface_size; ++i) {
      if (strcmp(_iface_cfg[i].iface, iface) == 0) {
        conf = &_iface_cfg[i].conf;
        break;
      }
    }
  }

  return conf;
}

static void _init_iface() {
  if (_iface_cfg != nullptr) {
    return;
  }

  const char *iface_list[] {
#if ESP32 || ESP_PLATFORM || ESP8266 || __APPLE__ || __linux__
    "wlan0",
#endif
#if ARDUINO_WT32_ETH01 || ETH_ENABLE
        "eth0",
#endif
  };

  _iface_size = sizeof(iface_list) / sizeof(const char *);
  _iface_cfg = new _net_iface_t[_iface_size];

  for (int i = 0; i < _iface_size; ++i) {
    _iface_cfg[i].iface = iface_list[i];
    _iface_cfg[i].conf.address = {0, 0, 0, 0};
    _iface_cfg[i].conf.gateway = {0, 0, 0, 0};
    _iface_cfg[i].conf.mask_bit = 32;
  }
}

static bool _is_default_config(const net_config_t &cfg) {
  net_config_t default_cfg = NET_DEFAULT_CFG;
  return memcmp(&cfg, &default_cfg, sizeof(net_config_t));
}

void _process_option(const char *iface, const char *option, const char *arg) {
  auto config = _net_config_get(iface);
  struct {
    const char *name;
    int has_arg;
    int val;
  } cf_configs[]{
      {"static", required_argument, 'S'},
  };

  if (config == nullptr) {
    return;
  }

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
    _process_opt_static(*config, arg);
    break;

  default:
    break;
  }
}

void _process_opt_static(net_config_t &conf, const char *arg) {
  if (strstr(arg, "ip_address") != nullptr) {
    int mask_bit;
    int ipv4[4];
    int count = sscanf(arg, "ip_address=%d.%d.%d.%d/%d", &ipv4[3], &ipv4[2],
                       &ipv4[1], &ipv4[0], &mask_bit);
    if (count == 5 && mask_bit >= 0 && mask_bit <= 32) {
      conf.address.ipv4[0] = ipv4[0];
      conf.address.ipv4[1] = ipv4[1];
      conf.address.ipv4[2] = ipv4[2];
      conf.address.ipv4[3] = ipv4[3];
      conf.mask_bit = mask_bit;
    }
  } else if (strstr(arg, "routers") != nullptr) {
    int ipv4[4];
    int count = sscanf(arg, "routers=%d.%d.%d.%d", &ipv4[3], &ipv4[2], &ipv4[1],
                       &ipv4[0]);

    conf.gateway.ipv4[0] = ipv4[0];
    conf.gateway.ipv4[1] = ipv4[1];
    conf.gateway.ipv4[2] = ipv4[2];
    conf.gateway.ipv4[3] = ipv4[3];
  }
}
