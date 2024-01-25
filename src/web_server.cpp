/** See a brief introduction (right-hand button) */
#include "web_server.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
AsyncWebsocketHelper *AsyncWebsocketHelper::_ws_event_receiver = nullptr;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void notFound(AsyncWebServerRequest *request);

/* Private function ----------------------------------------------------------*/
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void AsyncWebsocketHelper::_ws_event_sender(AsyncWebSocket *server,
                                            AsyncWebSocketClient *client,
                                            AwsEventType type, void *arg,
                                            uint8_t *data, size_t len) {
  if (_ws_event_receiver != nullptr) {
    _ws_event_receiver->_on_ws_event(client, type, arg, data, len);
  }
}

/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
AsyncWebsocketHelper::AsyncWebsocketHelper() : AsyncWebSocket("/ws") {
  if (_ws_event_receiver == nullptr) {
    _ws_event_receiver = this;
    onEvent(_ws_event_sender);
  } else {
    log_e("currently only one event sender could be used.");
  }
}

WebServer::WebServer() : AsyncWebServer(80) {
  addHandler(this);
  onNotFound(notFound);
}

void WebServer::_on_ws_event(AsyncWebSocketClient *client, AwsEventType type,
                             void *arg, uint8_t *data, size_t len) {

  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    // handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}
