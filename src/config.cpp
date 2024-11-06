/** See a brief introduction (right-hand button) */
#include "config.h"
/* Private include -----------------------------------------------------------*/
#include <ArduinoJson.h>
#include <LittleFS.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONF_MAX_SIZE 512 /** 1k */

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static BoxRingScreenCFG _box_rs_cfg{IPAddress{}, 3333};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
BoxRingScreenCFG init_rs_cfg() {
  char buf[CONF_MAX_SIZE]{0};

  /** file should exists. */
  auto path = "/etc/box_rs.json";
  if (LittleFS.exists(path)) {
    /** open file */
    File file = LittleFS.open(path, FILE_READ);
    file.readBytes(buf, sizeof(buf));
    file.close();
  }

  JsonDocument rs_cfg;
  deserializeJson(rs_cfg, buf, CONF_MAX_SIZE);

  if (rs_cfg.containsKey("ip")) {
    _box_rs_cfg.ip = IPAddress(rs_cfg["ip"].as<uint32_t>());
    _box_rs_cfg.ip_str = _box_rs_cfg.ip.toString();
  }

  if (rs_cfg.containsKey("port")) {
    _box_rs_cfg.port = IPAddress(rs_cfg["port"].as<int>());
  }

  if (rs_cfg.containsKey("index")) {
    _box_rs_cfg.index = IPAddress(rs_cfg["index"].as<int>());
  }

  return _box_rs_cfg;
}

BoxRingScreenCFG rs_cfg() { return _box_rs_cfg; }

void update_rs_cfg(IPAddress ip, int port, int index) {
  String output;
  auto path = "/etc/box_rs.json";
  JsonDocument rs_cfg;

  _box_rs_cfg.ip = ip;
  _box_rs_cfg.ip_str = ip.toString();
  _box_rs_cfg.port = port;
  _box_rs_cfg.index = index;

  rs_cfg["ip"] = static_cast<uint32_t>(ip);
  rs_cfg["port"] = port;
  rs_cfg["index"] = index;

  File file = LittleFS.open(path, FILE_WRITE);
  serializeJson(rs_cfg, output);
  file.print(output.c_str());
  file.close();
}