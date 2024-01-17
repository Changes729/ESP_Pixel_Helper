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
#include <NeoPixelBus.h>

#include "network_manager.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
NetworkManager network_manager;

const uint16_t PixelCount = 16 * 16;
const uint8_t PixelPin =
    32; // make sure to set this to the correct pin, ignored for Esp8266

#define colorSaturation 128

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

RgbColor current_color;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  log_d("opcode: %d", info->opcode);
  char hex[3]{0};
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    hex[0] = data[1];
    hex[1] = data[2];
    sscanf(hex, "%x", &current_color.R);
    hex[0] = data[3];
    hex[1] = data[4];
    sscanf(hex, "%x", &current_color.G);
    hex[0] = data[5];
    hex[1] = data[6];
    sscanf(hex, "%x", &current_color.B);
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
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
    const char *_AP_SSID = "ESP_%s";
    const char *_AP_PASSWD = "ESP3223PSE";
    size_t len = snprintf(NULL, 0, _AP_SSID, WiFi.macAddress().c_str()) + 1;
    char ssid[len];
    snprintf(ssid, len, _AP_SSID, WiFi.macAddress().c_str());

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig({192, 168, 0, 1}, {192, 168, 0, 1}, {255, 255, 255, 0});
    WiFi.softAP(ssid, _AP_PASSWD);

    IPAddress myIP = network_manager.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
  }
  // this resets all the neopixels to an off state
  strip.Begin();
  strip.Show();

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
    network_manager.update_dhcpcd("/etc/dhcpcd.conf");
    request->send(200);
  });

  // curl -d "ssid=_box&passwd=Wifimima8nengwei0" -H "Content-Type:
  // application/x-www-form-urlencoded" -X POST
  // http://192.168.1.1/api/wpa_supplicant/
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
              network_manager.update_wpa_supplicant("/etc/wpa_supplicant.conf");

              request->send(200);
            });

  server.onNotFound(notFound);

  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.begin();
  log_n("heap size: 0x%X", esp_get_free_heap_size());
}

void loop() {
#if 0
  // set the colors,
  // if they don't match in order, you need to use NeoGrbFeature feature
  strip.SetPixelColor(0, red);
  strip.SetPixelColor(1, green);
  strip.SetPixelColor(2, blue);
  strip.SetPixelColor(3, white);
  // the following line demonstrates rgbw color support
  // if the NeoPixels are rgbw types the following line will compile
  // if the NeoPixels are anything else, the following line will give an error
  // strip.SetPixelColor(3, RgbwColor(colorSaturation));
  strip.Show();

  delay(5000);

  // turn off the pixels
  strip.SetPixelColor(0, black);
  strip.SetPixelColor(1, black);
  strip.SetPixelColor(2, black);
  strip.SetPixelColor(3, black);
  strip.Show();

  delay(5000);

  // set the colors,
  // if they don't match in order, you may need to use NeoGrbFeature feature
  strip.SetPixelColor(0, hslRed);
  strip.SetPixelColor(1, hslGreen);
  strip.SetPixelColor(2, hslBlue);
  strip.SetPixelColor(3, hslWhite);
  strip.Show();

  delay(5000);

  // turn off the pixels
  strip.SetPixelColor(0, hslBlack);
  strip.SetPixelColor(1, hslBlack);
  strip.SetPixelColor(2, hslBlack);
  strip.SetPixelColor(3, hslBlack);
  strip.Show();
#elif 1
  strip.ClearTo(current_color);
  strip.Show();
  delay(10);
#endif
}