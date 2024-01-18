/** See a brief introduction (right-hand button) */
#include "network_manager.h"
/* Private include -----------------------------------------------------------*/
#if ESP32 || ESP8266
#include <LittleFS.h>
#endif

/* Private namespace ---------------------------------------------------------*/
using namespace fs;

/* Private define ------------------------------------------------------------*/
#define NET_CONF_MAX_SIZE 1024 /** 1k */

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */

NetworkManager::NetworkManager()
    : _iface_eth{"eth0", NET_DEFAULT_CFG},
      _iface_wlan{"wlan0", NET_DEFAULT_CFG}, _wifi_configs{} {}

NetworkManager::~NetworkManager() {}

wl_status_t NetworkManager::begin() {
  wl_status_t status = WL_NO_SHIELD;
  for (int i = 0; i < WIFI_CONFIGS_MAX; ++i) {
    auto &config = _wifi_configs[i];
    if (!config.ssid.isEmpty()) {
      log_n("connect to ssid: %s\n", config.ssid);
      status = WiFiSTAClass::begin(config.ssid, config.passwd);
      waitForConnectResult(20000);
      if (WiFiSTAClass::isConnected()) {
        break;
      }
    }
  }

  return status;
}

bool NetworkManager::config_dhcpcd(const char *path) {
#if __APPLE__ || __linux__
  /** read file */
  char buf[]{"interface wlan0\n"
             "static ip_address=192.168.1.23/24\n"
             "static routers=192.168.1.1\n"
             "static domain_name_servers=192.168.1.1\n"};

#else
  char buf[NET_CONF_MAX_SIZE]{0};

  /** file should exists. */
  if (LittleFS.exists(path)) {
    /** open file */
    File file = LittleFS.open(path, FILE_READ);
    file.readBytes(buf, sizeof(buf));
    file.close();
  }
#endif

  net_config_load(buf, _iface_eth.iface, &_iface_eth.conf);
  net_config_load(buf, _iface_wlan.iface, &_iface_wlan.conf);
  return 0;
}

bool NetworkManager::config_wpa_supplicant(const char *path) {
#if __APPLE__ || __linux__
  /** read file */
  char buf[]{"network={\n"
             "ssid=\"your-networks-SSID\"\n"
             "psk=\"your-networks-password\"\n"
             "}"};

#else
  char buf[NET_CONF_MAX_SIZE]{0};

  /** file should exists. */
  if (LittleFS.exists(path)) {
    /** open file */
    File file = LittleFS.open(path, FILE_READ);
    file.readBytes(buf, sizeof(buf));
    file.close();
  }
#endif

  return wifi_config_load(buf, _wifi_configs, WIFI_CONFIGS_MAX);
}

bool NetworkManager::update_dhcpcd(const char *path) {
  char buf[NET_CONF_MAX_SIZE]{0};
  size_t used = 0;
  used += net_config_print(buf, NET_CONF_MAX_SIZE - used, _iface_eth.iface,
                           &_iface_eth.conf);
  used += net_config_print(buf, NET_CONF_MAX_SIZE - used, _iface_wlan.iface,
                           &_iface_wlan.conf);

#if __APPLE__ || __linux__
#else
  File file = LittleFS.open(path, FILE_WRITE);
  file.print(buf);
  file.close();
#endif

  return used;
}

bool NetworkManager::update_wpa_supplicant(const char *path) {
  char buf[NET_CONF_MAX_SIZE]{0};
  size_t used = 0;
  used += wifi_config_print(buf, NET_CONF_MAX_SIZE - used, _wifi_configs,
                            WIFI_CONFIGS_MAX);

#if __APPLE__ || __linux__
#else
  File file = LittleFS.open(path, FILE_WRITE);
  file.print(buf);
  file.close();
#endif

  return used;
}
