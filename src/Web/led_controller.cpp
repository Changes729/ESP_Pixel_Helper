/** See a brief introduction (right-hand button) */
#include "led_controller.h"
/* Private include -----------------------------------------------------------*/
#include "system_operation.h"

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define this WebPixelController::instance()

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
WebPixelController::WebPixelController()
    : Websocket("/ws/led", _ws_handler), _strip{PIXEL_COUNT, PIXEL_PIN} {
  _strip.Begin();
  _strip.Show();
}

void WebPixelController::_ws_handler(AsyncWebSocket *server,
                                     AsyncWebSocketClient *client,
                                     AwsEventType type, void *arg,
                                     uint8_t *data, size_t len) {

  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA: {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    log_d("opcode: %d", info->opcode);
    char hex[3]{0};
    if (info->final && info->index == 0 && info->len == len &&
        info->opcode == WS_TEXT) {
      data[len] = 0;
      hex[0] = data[1];
      hex[1] = data[2];
      sscanf(hex, "%x", &this._current_color.R);
      hex[0] = data[3];
      hex[1] = data[4];
      sscanf(hex, "%x", &this._current_color.G);
      hex[0] = data[5];
      hex[1] = data[6];
      sscanf(hex, "%x", &this._current_color.B);
    }

    SYSTEM::notify(SYSTEM::LED_UPDATE);
  } break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void WebPixelController::flush() {
  _strip.ClearTo(_current_color);
  _strip.Show();
}
