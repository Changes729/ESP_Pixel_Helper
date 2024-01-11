/** See a brief introduction (right-hand button) */
#include "wifi_config.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#if ESP32 || ESP8266
#include <LittleFS.h>
#endif

#include "wifi_common.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NET_CONF_MAX_SIZE 1024 /** 1k */

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static wifi_config *_wifi_config = nullptr;
static size_t _config_size = 0;
static wifi_config *_wifi_handler = nullptr;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void _init_config(size_t config_count);
static void _process_option(const char *iface, const char *option,
                            const char *arg);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
wifi_config::wifi_config(const char *ssid, const char *passwd)
    : _ssid{ssid ? strdup(ssid) : nullptr},
      _passwd(passwd ? strdup(passwd) : nullptr) {}

wifi_config::wifi_config(wifi_config &&right)
    : _ssid(nullptr), _passwd(nullptr) {
  std::swap(_ssid, right._ssid);
  std::swap(_passwd, right._passwd);
}

wifi_config::~wifi_config() {
  free(_ssid);
  free(_passwd);
}

void wifi_config::set_ssid(const char *ssid) {
  free(_ssid);
  _ssid = ssid ? strdup(ssid) : nullptr;
}

void wifi_config::set_passwd(const char *passwd) {
  free(_passwd);
  _passwd = passwd ? strdup(passwd) : nullptr;
}

const char *wifi_config::get_ssid() const { return _ssid; }

const char *wifi_config::get_passwd() const { return _passwd; }

bool wifi_config::is_valid() const {
  return _ssid != nullptr && _passwd != nullptr;
}

bool wifi_config::is_ssid_equal(const char *ssid) const {
  bool same = false;
  if (is_valid() && ssid != nullptr) {
    same = strcmp(ssid, _ssid);
  }
  return false;
}

void wifi_config::clear() {
  free(_ssid);
  free(_passwd);
  _ssid = nullptr;
  _passwd = nullptr;
}

size_t wifi_config_count() { return _config_size; }

const wifi_config *wifi_config_get(size_t index) {
  return index < _config_size ? &_wifi_config[index] : nullptr;
}

int wifi_config_set(const char *ssid, const char *passwd) {
  int ret = false;
  if (_config_size != 0) {
    _wifi_config[0].set_ssid(ssid);
    _wifi_config[0].set_passwd(passwd);
    ret = true;
  }
  return ret;
}

int wifi_config_clear(const char *ssid) {
  for (int i = 0; i < _config_size; ++i) {
    if (_wifi_config[i].is_ssid_equal(ssid)) {
      _wifi_config[i].clear();
      break;
    }
  }

  return 0;
}

int wifi_config_init(const char *f) {
  int ret = 0;

  do {
#if __APPLE__ || __linux__
    /** read file */
    char buf[NET_CONF_MAX_SIZE]{"network={\n"
                                "    ssid=\"your-networks-SSID\"\n"
                                "    psk=\"your-networks-password\"\n"
                                "}"};
    _init_config(wifi_resolver::count_settings(buf));
    wifi_resolver{buf, strlen(buf)}.resolve(_process_option);
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
      _init_config(wifi_resolver::count_settings(buf));
      wifi_resolver{buf, buf_size}.resolve(_process_option);
    }
    file.close();
#endif
  } while (0);

  return ret;
}

int wifi_config_update(const char *f) {
  assert(_wifi_config != nullptr && f != nullptr);
  if (_wifi_config == nullptr || f == nullptr) {
    return 0;
  }

#if __APPLE__ || __linux__
#else
  File config_file = LittleFS.open(f, "w");
  for (int i = 0; i < _config_size; ++i) {
    if (!_wifi_config[i].is_valid()) {
      continue;
    }

    config_file.println("network={");

    config_file.print("ssid=\"");
    config_file.print(_wifi_config[i].get_ssid());
    config_file.println("\"");

    config_file.print("psk=\"");
    config_file.print(_wifi_config[i].get_passwd());
    config_file.println("\"");

    config_file.println("}");
  }
  config_file.close();
#endif

  return 0;
}

int wifi_config_deinit() {
  delete[] _wifi_config;
  _wifi_config = nullptr;
  _config_size = 0;
  return 0;
}

static void _init_config(size_t config_count) {
  if (_config_size == config_count) {
    return;
  }

  wifi_config_deinit();
  _wifi_config = new wifi_config[config_count];
  _config_size = config_count;
  _wifi_handler = nullptr;
}

static void _process_option(const char *settings, const char *option,
                            const char *arg) {
  assert(settings != nullptr);
  if (strcmp(settings, "network") == 0) {
    char *str = strdup(arg);
    char *handler = str;
    const char *record = nullptr;
    const char *param = nullptr;

    if (strcmp(option, "network") == 0) {
      if (_wifi_handler == nullptr) {
        _wifi_handler = _wifi_config;
      } else {
        _wifi_handler = _wifi_handler + 1;
      }
    } else if (strcmp(option, "ssid") == 0) {
      for (int i = 0; handler[i] != '\0'; ++i) {
        if (handler[i] == '\"') {
          handler[i] = '\0';
          if (record == nullptr) {
            record = handler + 1;
          } else {
            param = record;
            break;
          }
        }
      }

      _wifi_handler->set_ssid(param);
    } else if (strcmp(option, "psk") == 0) {
      for (int i = 0; handler[i] != '\0'; ++i) {
        if (handler[i] == '\"') {
          handler[i] = '\0';
          if (record == nullptr) {
            record = handler + 1;
          } else {
            param = record;
            break;
          }
        }
      }

      _wifi_handler->set_passwd(param);
    }

    free(str);
  }
}