/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H
#pragma once
/* Public include ------------------------------------------------------------*/
/* Public namespace ----------------------------------------------------------*/
namespace SYSTEM {

/* Public define -------------------------------------------------------------*/
#define DHCP_CFG_PATH "/etc/dhcpcd.conf"
#define WPA_CFG_PATH "/etc/wpa_supplicant.conf"

/* Public typedef ------------------------------------------------------------*/
typedef enum {
  NONE = 0,
  WPA_UPDATE,
  DHCP_UPDATE,
} NOTIFY_INDEX;

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
void init();
void loop();

void notify(NOTIFY_INDEX index);

};     // namespace Signal
#endif /* SYSTEM_MANAGER_H */
