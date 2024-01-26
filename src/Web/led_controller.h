/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "Language/instance.h"

#include "web_server.h"
#include <NeoPixelBus.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define PIXEL_COUNT 1
#define PIXEL_PIN 32

/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/

class WebPixelController : public Instance<WebPixelController>,
                           public Websocket {
private:
  friend Instance<WebPixelController>;
  WebPixelController();

public:
  void flush();

private:
  static void _ws_handler(AsyncWebSocket *server, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data,
                          size_t len);

private:
  NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> _strip;
  RgbColor _current_color;
};

#endif /* LED_CONTROLLER_H */
