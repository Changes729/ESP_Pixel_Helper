/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <ETH.h>
#include <stdint.h>

#include "IPAddress.h"
#include "IPv6Address.h"
#include "Print.h"

#include "WiFiAP.h"
#include "WiFiGeneric.h"
#include "WiFiSTA.h"
#include "WiFiScan.h"
#include "WiFiType.h"

#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"

#include "Network/net_config.h"
#include "Network/wifi_config.h"

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
#define WIFI_CONFIGS_MAX 3

/* Public typedef ------------------------------------------------------------*/
typedef struct _net_interface {
  const char *iface;
  net_config_t conf;
} net_iface_t;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class WiFiGenericEventHelper : public WiFiGenericClass {
public:
  WiFiGenericEventHelper();

protected:
  virtual void _on_wifi_event(WiFiEvent_t event) = 0;

private:
  static void _wifi_event_sender(WiFiEvent_t event);
  static WiFiGenericEventHelper *_wifi_event_receiver;
};

class ETHClass_ext : public ETHClass {
public:
  ETHClass_ext();

  inline bool is_connect() { return _eth_connected; }
  uint8_t waitForConnectResult(unsigned long timeoutLength = 60000);

protected:
  void _on_eth_event(WiFiEvent_t event);

private:
  bool _eth_connected;
};

class NetworkManager : public WiFiGenericEventHelper,
                       public WiFiSTAClass, /** station */
                       public WiFiScanClass,
                       public WiFiAPClass,
                       public ETHClass_ext {
public:
  NetworkManager();
  virtual ~NetworkManager();

  wl_status_t begin();
  bool config_dhcpcd(const char *file);
  bool config_wpa_supplicant(const char *file);

  bool update_dhcpcd(const char *file);
  bool update_wpa_supplicant(const char *file);

  IPAddress localIP();
  void startAP();

protected:
  virtual void _on_wifi_event(WiFiEvent_t event);

private:
  net_iface_t _iface_eth;
  net_iface_t _iface_wlan;
  wifi_ap_t _wifi_configs[WIFI_CONFIGS_MAX];
};

#endif /* NETWORK_MANAGER_H */
