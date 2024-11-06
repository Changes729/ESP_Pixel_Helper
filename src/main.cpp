/** See a brief introduction (right-hand button) */
/* Private include -----------------------------------------------------------*/
#include "config.h"

#include <Arduino.h>
#include <ElegantOTA.h>
#include <LittleFS.h>
#include <USB.h>
#include <USBHIDGamepad.h>

#ifdef ESP32
#include <AsyncTCP.h>

#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include "Module/encoder.h"
#include "Module/gamepad.h"
#include "Web/network_settings.h"
#include "config.h"
#include "network_manager.h"
#include "system_operation.h"
#include "web_server.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ENCODER_A 13
#define ENCODER_B 14
#define HANDLED 0

/* Private typedef -----------------------------------------------------------*/
typedef int (*_command_handler)(int argc, const char *argv[]);

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned long _disconnect_ts = 0;
static unsigned long _encoder_ts = 0;
static int _curr_angle = 0;
static uint16_t _last_touched;
static unsigned long _send_cycle = 0;
constexpr unsigned long _SEND_TS = 100; /* 200 ms */

/** Debug part -----------------------------------------------------*/
static bool _debug_enable = false;
static bool _sys_enable = true;
static IPAddress _debug_ip{0, 0, 0, 0};
static String _debug_ip_str = _debug_ip.toString();
static uint32_t _debug_port = 3333;
static Joystick_ Joystick;
int lastButtonState = 0;

static size_t _generate_argv_with_buffer(const char *argv[], char *buffer);

static int _handle_set_data(int argc, const char *argv[]);
static int _handle_get_status(int argc, const char *argv[]);
static int _handle_test(int argc, const char *argv[]);

static const struct {
  const char *cmd_name;
  _command_handler cb;
} _COMMAND_CALLBACKS[] = {
    {"set", _handle_set_data},
    {"get", _handle_get_status},
    {"test", _handle_test},
};

/** GamePad part ---------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void check_network_state();
static void send_box_msg(const String &str);
static void send_msg(const String &str, const char *server_ip, int server_port);

static bool _task_read_serial(void *_);
static void handle_command(int argc, const char *argv[]);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
void setup() {
  /** Params init */
  WebServer::create();
  NetworkManager::create();

  /** Serial init */
  Serial.begin(115200);

  /** LittleFS init */
  if (!LittleFS.begin(false, "")) {
    Serial.println("LittleFS mount failed");
    return;
  }

  SYSTEM::init();
  init_rs_cfg();
#if 0
  NetworkManager::instance().config_dhcpcd("/etc/dhcpcd.conf");
  NetworkManager::instance().config_wpa_supplicant("/etc/wpa_supplicant.conf");
  NetworkManager::instance().begin();

  // this resets all the neopixels to an off state
  WebServer::instance().on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hello, world");
  });

  NetworkSettings::init();
  WebServer::instance().begin();
  ElegantOTA.begin(&WebServer::instance());
#endif
  encoder_init(ENCODER_A, ENCODER_B);
  GamePad::create();
}

void loop() {
  SYSTEM::loop();
  _task_read_serial(NULL);
#if 0
  ElegantOTA.loop();
  check_network_state();
#endif

  bool could_send_msg = false;
  if (millis() - _send_cycle > _SEND_TS) {
    could_send_msg = true;
    _send_cycle = millis();
  }

  GamePad::instance().update();

  if (could_send_msg) {
    {
      // get encoder count and clean up.
      long increment = get_encoder_count(true) % ENCODER_EDGE;
      if (increment) {
        // send_box_msg("_cobox_point_" + String(increment));
        Serial.println(increment);
        _encoder_ts = millis();
      }

      if (millis() - _encoder_ts > ENCODER_WATCH_UP) {
        _curr_angle = 0;
      } else if (increment) {
        _curr_angle += increment;
        _encoder_ts = millis();
      }
    }
  }
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

static size_t _generate_argv_with_buffer(const char **argv, char *buffer) {
  size_t argc = 0;
  char *ch = buffer;

  while (*ch != '\0') {
    if (ch == buffer) {
      if (!isgraph(*ch)) {
        buffer = ch + 1;
      } else {
        if (nullptr != argv) {
          argv[argc] = ch;
        }
        argc += 1;
      }
    } else if (!isgraph(*ch) && isgraph(*(ch - 1))) {
      if (nullptr != argv) {
        *ch = '\0';
      }
      buffer = ch + 1;
    }
    ch += 1;
  }

  return argc;
}

static bool _task_read_serial(void *_) {
  constexpr uint32_t BUFFER_SIZE = 512;
  static char buffer[BUFFER_SIZE]{'\0'};
  static size_t buffer_handler = 0;
  while (Serial.available()) {
    buffer[buffer_handler] = Serial.read();
    if (buffer[buffer_handler] == '\n') {
      buffer[buffer_handler] = '\0';
      log_d("Get command %s", buffer);
      size_t argc = _generate_argv_with_buffer(NULL, buffer);
      const char *argv[argc];
      _generate_argv_with_buffer(argv, buffer);
      log_d("get %u argment:", argc);
      for (size_t i = 0; i < argc; ++i) {
        log_d("\t%s", argv[i]);
      }
      handle_command(argc, argv);

      buffer_handler = 0;
    } else if (buffer_handler < BUFFER_SIZE - 1) {
      buffer_handler += 1;
    } else {
      log_e("Command is too long.");
      buffer_handler = 0;
    }
  }

  return true;
}

static void handle_command(int argc, const char *argv[]) {
  bool is_handled = !HANDLED;
  if (argc > 0) {
    for (auto &record : _COMMAND_CALLBACKS) {
      if (strcmp(argv[0], record.cmd_name) == 0) {
        is_handled = record.cb(argc - 1, argv + 1);
        break;
      }
    }

    if (HANDLED != is_handled) {
      log_e("Error handle command.");
    }
  }
}

static int _handle_set_data(int argc, const char *argv[]) {
  int handled = HANDLED;
  if (argc <= 0) {
    handled = !HANDLED;
  } else if (strcmp(argv[0], "index") == 0) {
    auto cfg = rs_cfg();
    if (argc < 2) {
      handled = !HANDLED;
    } else {
      int index = atoi(argv[1]);
      update_rs_cfg(cfg.ip, cfg.port, index);
      Serial.printf("[index] ok\n");
    }
  }
  return handled;
}

static int _handle_get_status(int argc, const char *argv[]) {
  int handled = HANDLED;
  if (argc <= 0) {
    handled = !HANDLED;
  } else if (strcmp(argv[0], "index") == 0) {
    Serial.printf("[index] %i\n", rs_cfg().index);
  }
  return handled;
}

static int _handle_test(int argc, const char *argv[]) {
  int handled = HANDLED;
  if (argc <= 0) {
    handled = !HANDLED;
  } else if (strcmp(argv[0], "hello") == 0) {
    Serial.println("Hello.");
  }
  return handled;
}
