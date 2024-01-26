/** See a brief introduction (right-hand button) */
#include "web_server.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void notFound(AsyncWebServerRequest *request);

/* Private function ----------------------------------------------------------*/
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

/* Private class function ----------------------------------------------------*/
Websocket::Websocket(const char *url, AwsEventHandler handler)
    : AsyncWebSocket(url) {
  onEvent(handler);
  WebServer::instance().addHandler(this);
}

/**
 * @brief  ...
 * @param  None
 * @retval None
 */
WebServer::WebServer() : AsyncWebServer(80) { onNotFound(notFound); }
