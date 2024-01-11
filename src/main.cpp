#include "config.h"

#include <Arduino.h>
#include <LittleFS.h>
#ifdef ESP32
#include <AsyncTCP.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#include "network_manager.h"

AsyncWebServer server(80);
NetworkManager network_manager;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  /** Params init */
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

  network_manager.config_dhcpcd("/etc/dhcpcd.conf");
  network_manager.config_wpa_supplicant("/etc/wpa_supplicant.conf");
  if (network_manager.begin() != WL_NO_SHIELD) {
    while (network_manager.status() != WL_CONNECTED) {
      delay(500);
      log_n(".");
    }

    log_n("\nWiFi connected\nIP address: %s\n",
          network_manager.localIP().toString().c_str());
  }

  if (network_manager.status() != WL_CONNECTED) {
    network_manager.softAP("ESP_32", "12345678");
    IPAddress myIP = network_manager.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }

  server.serveStatic("/", LittleFS, "/usr/share/mbedhttp/html/");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
  });

  server.on("/api/dhcpcd/", HTTP_POST, [](AsyncWebServerRequest *request) {
    int params = request->params();
    IPAddress ip;
    IPAddress gateway;
    IPAddress mask;
    for (int i = 0; i < params; i++) {
      AsyncWebParameter *p = request->getParam(i);
      if (p->isPost()) {
        if (p->name() == "ip") {
          ip.fromString(p->value());
        } else if (p->name() == "gateway") {
          gateway.fromString(p->value());
        } else if (p->name() == "mask") {
          mask.fromString(p->value());
        }
      }
    }

    uint32_t mask_32 = static_cast<uint32_t>(mask);
    uint8_t mask_bit_count = 0;
    for (int i = 0; i < 32; ++i) {
      if (mask_32 & 0x80000000 == 0x80000000) {
        mask_bit_count += 1;
      } else {
        break;
      }
      mask_32 <<= 1;
    }

    net_config_t config{
        .address = static_cast<uint32_t>(ip),
        .gateway = static_cast<uint32_t>(gateway),
        .mask_bit = mask_bit_count,
    };
    network_manager.update_dhcpcd("wlan0", config);
    request->send(200);
  });

  // curl -d "ssid=_box&passwd=Wifimima8nengwei0" -H "Content-Type: application/x-www-form-urlencoded" -X POST http://192.168.1.1/api/wpa_supplicant/
  server.on("/api/wpa_supplicant/", HTTP_POST,
            [](AsyncWebServerRequest *request) {
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
              network_manager.update_wpa_supplicant(ssid, passwd);

              request->send(200);
            });

  server.onNotFound(notFound);

  server.begin();
}

void loop() {}