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
class Websocket : protected AsyncWebSocket {
public:
  Websocket(const char *url, AwsEventHandler handler);
};

class WebServer : public Instance<WebServer>, public AsyncWebServer {
private:
  friend Instance<WebServer>;
  WebServer();
};

#endif /* WEB_SERVER_H */
