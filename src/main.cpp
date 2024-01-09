#include "config.h"

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>

#include "Network/net_config.h"
#include "Network/wifi_config.h"

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

  if (net_config_init("/etc/dhcpcd.conf") == 0) {
    auto config = net_config_get("wlan0");
    uint32_t subnet = 0xFFFFFFFF >> (32 - config->mask_bit);
#if DEBUG
    Serial.println(IPAddress(config->address));
    Serial.println(IPAddress(subnet));
#endif
    if (!WiFi.config(config->address, config->gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }
  }

  if (wifi_config_init("/etc/wpa_supplicant.conf") == 0) {
    auto config_size = wifi_config_count();
    for (int i = 0; i < config_size; ++i) {
      auto config = wifi_config_get(i);
      if (config->is_valid()) {
        auto ssid = config->get_ssid();
        auto passwd = config->get_passwd();
        WiFi.begin(ssid, passwd);

        while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        /** NOTE: connect first ssid currently. */
        break;
      }
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.softAP("ESP_32", "12345678");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
}

void loop() {}