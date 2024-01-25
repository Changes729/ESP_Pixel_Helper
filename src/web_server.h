/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WEB_SERVER_H
#define WEB_SERVER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <ESPAsyncWebServer.h>

#include "Language/instance.h"

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class AsyncWebsocketHelper : protected AsyncWebSocket {
public:
  AsyncWebsocketHelper();

protected:
  virtual void _on_ws_event(AsyncWebSocketClient *client, AwsEventType type,
                            void *arg, uint8_t *data, size_t len) = 0;

private:
  static AsyncWebsocketHelper *_ws_event_receiver;
  static void _ws_event_sender(AsyncWebSocket *server,
                               AsyncWebSocketClient *client, AwsEventType type,
                               void *arg, uint8_t *data, size_t len);
};

class WebServer : public Instance<WebServer>,
                  public AsyncWebServer,
                  public AsyncWebsocketHelper {
private:
  friend Instance<WebServer>;
  WebServer();

protected:
  void _on_ws_event(AsyncWebSocketClient *client, AwsEventType type, void *arg,
                    uint8_t *data, size_t len) override;
};

#endif /* WEB_SERVER_H */
