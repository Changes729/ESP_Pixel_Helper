/** See a brief introduction (right-hand button) */
#include "network_manager.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
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
    : _dhcpcd_config_file{nullptr}, _wpa_config_file{nullptr} {}

NetworkManager::~NetworkManager() {}

wl_status_t NetworkManager::begin() {
  wl_status_t status = WL_NO_SHIELD;
  auto config_size = wifi_config_count();
  for (int i = 0; i < config_size; ++i) {
    auto config = wifi_config_get(i);
    if (config->is_valid()) {
      auto ssid = config->get_ssid();
      auto passwd = config->get_passwd();
      log_n("connect to ssid: %s\n", ssid);
      status = WiFiSTAClass::begin(ssid, passwd);
      /** NOTE: connect first ssid currently. */
      break;
    }
  }

  return status;
}

bool NetworkManager::config_dhcpcd(const char *config_file) {
  bool ret = false;
  _dhcpcd_config_file = config_file;
  if (net_config_init(config_file) == 0) {
    auto conf = net_config_get("wlan0");
    uint32_t subnet = 0xFFFFFFFF >> (32 - conf->mask_bit);

    log_d(IPAddress(config->address));
    log_d(IPAddress(subnet));

    ret = WiFiSTAClass::config(conf->address, conf->gateway, subnet);
  }
  return ret;
}

bool NetworkManager::config_wpa_supplicant(const char *config_file) {
  _wpa_config_file = config_file;
  return wifi_config_init(config_file);
}

bool NetworkManager::update_dhcpcd(const char *iface,
                                   const net_config_t &net_config) {
  return net_config_set(iface, net_config) &&
         net_config_update(_dhcpcd_config_file);
}

bool NetworkManager::update_wpa_supplicant(const char *ssid,
                                           const char *passwd) {
  return wifi_config_set(ssid, passwd) && wifi_config_update("/etc/wpa_supplicant.conf");
}
