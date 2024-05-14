/** See a brief introduction (right-hand button) */
/* Private include -----------------------------------------------------------*/
#include "config.h"

#include <Arduino.h>
#include <FastLED.h>
#include <LittleFS.h>

#ifdef ESP32
#include <AsyncTCP.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include "Module/encoder.h"
#include "Module/nfc.h"
#include "Module/touch.h"
#include "Web/network_settings.h"
#include "config.h"
#include "network_manager.h"
#include "system_operation.h"
#include "web_server.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_SYS_STARTUP                                                        \
  { CRGB::Black, CRGB::Yellow, CRGB::Black }
#define LED_NETWORK_DOWN                                                       \
  { CRGB::Black, CRGB::Red, CRGB::Black }
#define LED_SYS_IDLE                                                           \
  { CRGB::Black, CRGB::Green, CRGB::Black }
#define LED_SYS_ACTIVE                                                         \
  { CRGB::Black, CRGB::Blue, CRGB::Black }

#define set_led(dest, src, size)                                               \
  do {                                                                         \
    CRGB led_[] = src;                                                         \
    memcpy(dest, led_, size * sizeof(CRGB));                                   \
  } while (0)

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned long _disconnect_ts = 0;
static unsigned long _encoder_ts = 0;
static int _curr_angle = 0;
static CRGB leds[NUM_LEDS] = LED_SYS_STARTUP;
static uint16_t _last_touched;

/** Debug part -----------------------------------------------------*/
static bool _debug_enable = false;
static bool _sys_enable = true;
static IPAddress _debug_ip{0, 0, 0, 0};
static String _debug_ip_str = _debug_ip.toString();
static uint32_t _debug_port = 3333;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void check_network_state();
static void light_init();
static void send_box_msg(const String &str);
static void send_msg(const String &str, const char *server_ip, int server_port);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
void setup() {
  /** Params init */
  WebServer::create();
  NetworkManager::create();

  /** Serial init */
  Serial.begin(115200);
  while (!Serial) {
  }

  /** LittleFS init */
  if (!LittleFS.begin(false, "")) {
    Serial.println("LittleFS mount failed");
    return;
  }

  SYSTEM::init();
  init_rs_cfg();

  NetworkManager::instance().config_dhcpcd("/etc/dhcpcd.conf");
  NetworkManager::instance().config_wpa_supplicant("/etc/wpa_supplicant.conf");
  NetworkManager::instance().begin();

  // this resets all the neopixels to an off state

  WebServer::instance().on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
  });

  NetworkSettings::init();
  WebServer::instance().begin();
  Touch::touch_init();
  light_init();

  encoder_init(ENCODER_PIN_A, ENCODER_PIN_B);
  nfc_init();
}

void loop() {
  SYSTEM::loop();
  check_network_state();

  /**
   * DOC: led_enable currently same as 'system_enable';
   * don't show any LED's and send information.
   *
   * allowed network work currently.
   */
  if (!_sys_enable) {
    FastLED.clear();
    FastLED.show();
    return;
  }

  do {
    if (NetworkManager::instance().is_connect() ||
        NetworkManager::instance().isConnected()) {
      String uid;
      if (has_uid(uid)) {
        log_n("send package");
        send_box_msg("_cobox_scene_" + uid);
        set_led(leds, LED_SYS_ACTIVE, NUM_LEDS);
      } else {
        set_led(leds, LED_SYS_IDLE, NUM_LEDS);
        break;
      }
    } else {
      set_led(leds, LED_NETWORK_DOWN, NUM_LEDS);
      if (!Touch::is_touch_started()) {
        leds[0] = CRGB::Red;
        leds[2] = CRGB::Red;
      }
      break;
    }

    /** on network connected. */
    if (Touch::is_touch_started()) {
      uint16_t curr_touched = Touch::check_touch_state();
      auto is_touched = [&](uint8_t i) {
        return (curr_touched & _BV(i)) && !(_last_touched & _BV(i));
      };
      auto is_released = [&](uint8_t i) {
        return !(curr_touched & _BV(i)) && (_last_touched & _BV(i));
      };

      if (is_touched(TOUCH_LEFT)) {
        log_d("left touch");
      } else if (is_released(TOUCH_LEFT)) {
        send_box_msg("_cobox_point_left");
      }

      if (is_touched(TOUCH_RIGHT)) {
        log_d("right touch");
      } else if (is_released(TOUCH_RIGHT)) {
        send_box_msg("_cobox_point_right");
      }

      leds[0] = curr_touched & _BV(TOUCH_LEFT) ? CRGB::Blue : CRGB::Black;
      leds[2] = curr_touched & _BV(TOUCH_RIGHT) ? CRGB::Blue : CRGB::Black;
      _last_touched = curr_touched;

      if (_debug_enable) {
        char buffer[128];
        Touch::set_debug_info(buffer, sizeof(buffer));
        send_msg(buffer, _debug_ip_str.c_str(), _debug_port);
      }
    }

    {
      // get encoder count and clean up.
      long increment = get_encoder_count(true) % ENCODER_EDGE;
      if (increment / 4) {
        send_box_msg("_cobox_point_" + String(increment));
        _encoder_ts = millis();
      }

      if (millis() - _encoder_ts > ENCODER_WATCH_UP) {
        _curr_angle = 0;
      } else if (increment / 4) {
        _curr_angle += increment;
        _encoder_ts = millis();
        // send_box_msg("_cobox_point_" + String(_curr_angle));
      }

      int light_index = _curr_angle % 1024 * 0.35 / 15 + 1;
      if (_curr_angle != 0) {
        for (auto &led : leds) {
          led = CRGB::Black;
        }
      }
      leds[light_index > 0 ? light_index % 3 : 2 - abs(light_index) % 3] =
          CRGB::Blue;
    }

  } while (0);

  FastLED.show();
  update_nfc();
}

bool debug_enable() { return _debug_enable; }

void set_debug(bool enable) { _debug_enable = enable; }

bool system_enable() { return _sys_enable; }

void set_system(bool enable) { _sys_enable = enable; }

IPAddress debug_ip() { return _debug_ip; }

uint32_t debug_port() { return _debug_port; }

void update_debug_client(IPAddress address, uint32_t port = 3333) {
  _debug_ip = address;
  _debug_ip_str = _debug_ip.toString();
  _debug_port = port;
}

static void light_init() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
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

static void send_box_msg(const String &str) {
  send_msg(str, rs_cfg().ip_str.c_str(), rs_cfg().port);
}

static void send_msg(const String &str, const char *server_ip,
                     int server_port) {
  assert(server_ip != nullptr);
  auto &udp = NetworkManager::instance();
  udp.beginPacket(server_ip, server_port);
  udp.printf("%s\n", str.c_str());
  udp.endPacket();
}
