/** See a brief introduction (right-hand button) */
#include "network_settings.h"
/* Private include -----------------------------------------------------------*/
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "AsyncJson.h"
#include "config.h"
#include "network_manager.h"
#include "system_operation.h"
#include "web_server.h"

/* Private namespace ---------------------------------------------------------*/
namespace NetworkSettings {

/* Private define ------------------------------------------------------------*/
#define server WebServer::instance()
#define network_manager NetworkManager::instance()

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void _async_wpa_info(AsyncWebServerRequest *request);
static void _async_eth_info(AsyncWebServerRequest *request);
static void _async_wlan_info(AsyncWebServerRequest *request);
static void _async_box_info(AsyncWebServerRequest *request);

static void _async_wpa_update(AsyncWebServerRequest *request, uint8_t *data,
                              size_t len, size_t index, size_t total);
static void _async_eth_update(AsyncWebServerRequest *request, uint8_t *data,
                              size_t len, size_t index, size_t total);
static void _async_wlan_update(AsyncWebServerRequest *request, uint8_t *data,
                               size_t len, size_t index, size_t total);

static void _async_box_cfg(AsyncWebServerRequest *request, uint8_t *data,
                           size_t len, size_t index, size_t total);

static void _async_body_handler(AsyncWebServerRequest *request) {
  request->send(204);
}

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
void init() {
  server.serveStatic("/", LittleFS, "/usr/share/mbedhttp/html/");
  server.on("/api/dhcpcd/eth", HTTP_GET, _async_eth_info);
  server.on("/api/dhcpcd/wlan", HTTP_GET, _async_wlan_info);
  server.on("/api/wpa_supplicant/info", HTTP_GET, _async_wpa_info);
  server.on("/api/box/info", HTTP_GET, _async_box_info);
  server.on("/api/dhcpcd/eth", HTTP_POST, _async_body_handler, NULL,
            _async_eth_update);
  server.on("/api/dhcpcd/wlan", HTTP_POST, _async_body_handler, NULL,
            _async_wlan_update);
  server.on("/api/wpa_supplicant/info", HTTP_POST, _async_body_handler, NULL,
            _async_wpa_update);
  server.on("/api/box/info", HTTP_POST, _async_body_handler, NULL,
            _async_box_cfg);
}

/**
 * @brief
 *
 * curl -d "ssid=_box&passwd=Wifimima8nengwei0" -H
 * "Content-Type:application/x-www-form-urlencoded" -X POST
 * http://192.168.1.1/api/wpa_supplicant/
 *
 * @param request
 */
static void _async_wpa_supplicant(AsyncWebServerRequest *request) {
  int params = request->params();
  const char *ssid = nullptr;
  const char *passwd = nullptr;
  for (int i = 0; i < params; i++) {
    AsyncWebParameter *p = request->getParam(i);
    if (p->isPost()) {
      if (p->name() == "ssid") {
        ssid = p->value().c_str();
      } else if (p->name() == "passwd") {
        passwd = p->value().c_str();
      }
    }
  }

  log_n("update ssid and passwd: %s %s\n", ssid, passwd);
  wifi_ap_t ap_list[] = {{ssid, passwd}};
  network_manager.update_wifi(ap_list, sizeof(ap_list) / sizeof(wifi_ap_t));
  network_manager.update_wpa_supplicant("/etc/wpa_supplicant.conf");

  request->send(200);
}

static void _async_wpa_info(AsyncWebServerRequest *request) {
  String output;
  JsonDocument wpa_info;
  JsonArray wifi_infos = wpa_info["metas"].to<JsonArray>();
  auto ap_list = network_manager.get_wifi_ap_list();

  wpa_info["max_info"] = WIFI_CONFIGS_MAX;
  for (int i = 0; i < network_manager.wifi_ap_count(); ++i) {
    if (!ap_list[i].ssid.isEmpty()) {
      JsonObject meta = wifi_infos.createNestedObject();
      meta["ssid"] = ap_list[i].ssid;
      meta["passwd"] = ap_list[i].passwd;
    }
  }

  serializeJson(wpa_info, output);
  request->send(200, "text/plain", output);
}

static void _async_eth_info(AsyncWebServerRequest *request) {
  String output;
  JsonDocument eth_info;
  auto eth = network_manager.get_eth_iface();

  eth_info["static_ip"] = IPAddress(eth.conf.address).toString();
  eth_info["gateway"] = IPAddress(eth.conf.gateway).toString();
  eth_info["mask"] = IPAddress(net_config_get_mask(eth.conf)).toString();

  serializeJson(eth_info, output);
  request->send(200, "text/plain", output);
}

static void _async_wlan_info(AsyncWebServerRequest *request) {
  String output;
  JsonDocument wlan_info;
  auto wlan = network_manager.get_wlan_iface();

  wlan_info["static_ip"] = IPAddress(wlan.conf.address).toString();
  wlan_info["gateway"] = IPAddress(wlan.conf.gateway).toString();
  wlan_info["mask"] = IPAddress(net_config_get_mask(wlan.conf)).toString();

  serializeJson(wlan_info, output);
  request->send(200, "text/plain", output);
}

static void _async_box_info(AsyncWebServerRequest *request) {
  String output;
  JsonDocument box_info;
  auto wlan = network_manager.get_wlan_iface();

  box_info["ip"] = rs_cfg().ip.toString();
  box_info["port"] = String(rs_cfg().port);

  serializeJson(box_info, output);
  request->send(200, "text/plain", output);
}

static void _async_wpa_update(AsyncWebServerRequest *request, uint8_t *data,
                              size_t len, size_t index, size_t total) {
  String output;
  JsonDocument wpa_info;
  deserializeJson(wpa_info, (const char *)data, total);
  auto array = wpa_info["metas"].as<JsonArray>();
  auto size = array.size();
  wifi_ap_t aps[size];
  for (int i = 0; i < size; ++i) {
    aps[i].ssid = array[i]["ssid"].as<String>();
    aps[i].passwd = array[i]["passwd"].as<String>();
  }

  network_manager.update_wifi(aps, size);
  request->send(204);

  SYSTEM::notify(SYSTEM::WPA_UPDATE);
}

static void _async_eth_update(AsyncWebServerRequest *request, uint8_t *data,
                              size_t len, size_t index, size_t total) {
  net_iface_t new_cfg;
  JsonDocument eth_info;
  auto ip_address = IPAddress();

  deserializeJson(eth_info, (const char *)data, total);
  new_cfg.iface = "eth0";
  ip_address.fromString(eth_info["static_ip"].as<const char *>());
  new_cfg.conf.address = static_cast<uint32_t>(ip_address);
  ip_address.fromString(eth_info["gateway"].as<const char *>());
  new_cfg.conf.gateway = static_cast<uint32_t>(ip_address);
  ip_address.fromString(eth_info["mask"].as<const char *>());
  new_cfg.conf.mask_bit = ip_to_mask(static_cast<uint32_t>(ip_address));
  network_manager.update_iface(new_cfg);
  request->send(204);

  SYSTEM::notify(SYSTEM::DHCP_UPDATE);
}

static void _async_wlan_update(AsyncWebServerRequest *request, uint8_t *data,
                               size_t len, size_t index, size_t total) {
  net_iface_t new_cfg;
  JsonDocument wlan_info;
  auto ip_address = IPAddress();

  deserializeJson(wlan_info, (const char *)data, total);
  new_cfg.iface = "wlan0";
  ip_address.fromString(wlan_info["static_ip"].as<const char *>());
  new_cfg.conf.address = static_cast<uint32_t>(ip_address);
  ip_address.fromString(wlan_info["gateway"].as<const char *>());
  new_cfg.conf.gateway = static_cast<uint32_t>(ip_address);
  ip_address.fromString(wlan_info["mask"].as<const char *>());
  new_cfg.conf.mask_bit = ip_to_mask(static_cast<uint32_t>(ip_address));
  network_manager.update_iface(new_cfg);
  request->send(204);

  SYSTEM::notify(SYSTEM::DHCP_UPDATE);
}

static void _async_box_cfg(AsyncWebServerRequest *request, uint8_t *data,
                           size_t len, size_t index, size_t total) {
  net_iface_t new_cfg;
  JsonDocument box_info;
  auto ip_address = IPAddress();

  deserializeJson(box_info, (const char *)data, total);
  request->send(204);

  ip_address.fromString(box_info["ip"].as<const char *>());
  update_rs_cfg(ip_address, atoi(box_info["port"].as<const char *>()));
}

}; // namespace NetworkSettings
