/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H
#pragma once
/* Public include ------------------------------------------------------------*/
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
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
/**
 * @brief copy implement from wifi.h.
 *
 */
class NetworkManager : public WiFiGenericClass,
                       public WiFiSTAClass, /** station */
                       public WiFiScanClass,
                       public WiFiAPClass {
public:
  NetworkManager();
  virtual ~NetworkManager();

  wl_status_t begin();
  bool config_dhcpcd(const char *config_file);
  bool config_wpa_supplicant(const char *config_file);

  bool update_dhcpcd(const char* iface, const net_config_t& net_config);
  bool update_wpa_supplicant(const char* ssid, const char* passwd);

private:
  const char* _dhcpcd_config_file;
  const char* _wpa_config_file;
};

#endif /* NETWORK_MANAGER_H */
