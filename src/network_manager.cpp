/** See a brief introduction (right-hand button) */
#include "network_manager.h"
/* Private include -----------------------------------------------------------*/
#include <LittleFS.h>

/* Private namespace ---------------------------------------------------------*/
using namespace fs;

/* Private define ------------------------------------------------------------*/
#define NET_CONF_MAX_SIZE 1024 /** 1k */

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
WiFiGenericEventHelper *WiFiGenericEventHelper::_wifi_event_receiver = nullptr;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
void WiFiGenericEventHelper::_wifi_event_sender(WiFiEvent_t event) {
  if (_wifi_event_receiver != nullptr) {
    _wifi_event_receiver->_on_wifi_event(event);
  }
}

/* Private class function ----------------------------------------------------*/
WiFiGenericEventHelper::WiFiGenericEventHelper() {
  if (_wifi_event_receiver == nullptr) {
    _wifi_event_receiver = this;
    onEvent(_wifi_event_sender);
  } else {
    log_e("currently only one event sender could be used.");
  }
}

ETHClass_ext::ETHClass_ext() : _eth_connected{false} {}

uint8_t ETHClass_ext::waitForConnectResult(unsigned long timeoutLength) {
  unsigned long start = millis();
  while ((!_eth_connected) && (millis() - start) < timeoutLength) {
    delay(100);
  }
  return _eth_connected;
}

void ETHClass_ext::_on_eth_event(WiFiEvent_t event) {
  switch (event) {
  case ARDUINO_EVENT_ETH_START:
    log_n("ETH Started");
    // set eth hostname here
    ETH.setHostname("esp32-ethernet");
    break;
  case ARDUINO_EVENT_ETH_CONNECTED:
    log_n("ETH Connected");
    break;
  case ARDUINO_EVENT_ETH_GOT_IP:
    log_n("ETH MAC: %s, IPv4: %s %s, %uMbps", ETH.macAddress().c_str(),
          ETH.localIP().toString().c_str(),
          ETH.fullDuplex() ? ", FULL_DUPLEX" : "", ETH.linkSpeed());
    _eth_connected = true;
    break;
  case ARDUINO_EVENT_ETH_DISCONNECTED:
    log_n("ETH Disconnected");
    _eth_connected = false;
    break;
  case ARDUINO_EVENT_ETH_STOP:
    log_n("ETH Stopped");
    _eth_connected = false;
    break;
  default:
    break;
  }
}

NetworkManager::NetworkManager()
    : _iface_eth{"eth0", NET_DEFAULT_CFG},
      _iface_wlan{"wlan0", NET_DEFAULT_CFG}, _wifi_configs{} {}

NetworkManager::~NetworkManager() {}

wl_status_t NetworkManager::begin() {
  wl_status_t status = WL_NO_SHIELD;

  /** NOTE: ETH configure should be set after eth::begin() */
  ETHClass_ext::begin();
  if (is_config_valid(_iface_eth.conf)) {
    ETHClass_ext::config(_iface_eth.conf.address, _iface_eth.conf.gateway,
                         net_config_get_mask(_iface_eth.conf));
  }

  for (int i = 0; i < WIFI_CONFIGS_MAX; ++i) {
    auto &config = _wifi_configs[i];
    if (!config.ssid.isEmpty()) {
      log_n("connect to ssid: %s\n", config.ssid);
      if (is_config_valid(_iface_wlan.conf)) {
        WiFiSTAClass::config(_iface_wlan.conf.address, _iface_wlan.conf.gateway,
                             net_config_get_mask(_iface_wlan.conf));
      }
      WiFiSTAClass::begin(config.ssid, config.passwd);
      WiFiSTAClass::waitForConnectResult(20000);
      status = WiFiSTAClass::status();
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

IPAddress NetworkManager::localIP() {
  return ETHClass_ext::is_connect() ? ETHClass_ext::localIP()
                                    : WiFiSTAClass::localIP();
}

void NetworkManager::startAP() {
#define SSID_HEAD "ESP_"
  char ssid[strlen(SSID_HEAD) + 18]{SSID_HEAD};
  char passwd[]{"ESP3223PSE"};
  char *ssid_mac = ssid + strlen(SSID_HEAD);
  wifi_get_mac(ssid_mac, 18);
  WiFiGenericClass::mode(WIFI_AP);
  softAPConfig({192, 168, 0, 1}, {192, 168, 0, 1}, {255, 255, 255, 0});
  softAP(ssid, passwd);
}

void NetworkManager::_on_wifi_event(WiFiEvent_t event) {
  ETHClass_ext::_on_eth_event(event);
}