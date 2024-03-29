#include "config.h"

#include <Arduino.h>
#include <LittleFS.h>
#ifdef ESP32
#include <AsyncTCP.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include "Web/network_settings.h"
#include "Web/led_controller.h"
#include "network_manager.h"
#include "system_operation.h"
#include "web_server.h"

static unsigned long _disconnect_ts = 0;
static void check_network_state();

void setup() {
  /** Params init */
  WebServer::create();
  NetworkManager::create();

  WebPixelController::create();

  /** Serial init */
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("start.");

  /** LittleFS init */
  if (!LittleFS.begin(false, "")) {
    Serial.println("LittleFS mount failed");
    return;
  }

  SYSTEM::init();

  NetworkManager::instance().config_dhcpcd("/etc/dhcpcd.conf");
  NetworkManager::instance().config_wpa_supplicant("/etc/wpa_supplicant.conf");
  NetworkManager::instance().begin();

  // this resets all the neopixels to an off state

  WebServer::instance().on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
  });

  NetworkSettings::init();
  WebServer::instance().begin();
}

void loop() {
  SYSTEM::loop();

  check_network_state();
}

static void check_network_state() {
  auto &network_manager = NetworkManager::instance();
  if (network_manager.getMode() == WIFI_AP) {
    /** nothing todo. */
  } else if (!network_manager.isConnected()) {
    if (_disconnect_ts == 0) {
      _disconnect_ts = millis();
    } else if (millis() - _disconnect_ts > 5000) {
      network_manager.startAP();
      _disconnect_ts = 0;
    }
  } else { /** if(network_manager.isConnected()) */
    _disconnect_ts = 0;
  }
}