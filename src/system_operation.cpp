/** See a brief introduction (right-hand button) */
#include "system_operation.h"
/* Private include -----------------------------------------------------------*/
#include <Arduino.h>

#include "network_manager.h"

/* Private namespace ---------------------------------------------------------*/
namespace SYSTEM {
/* Private define ------------------------------------------------------------*/
#define network_manager NetworkManager::instance()

/* Private typedef -----------------------------------------------------------*/
typedef struct _task {
  const NOTIFY_INDEX index;
  void (*const cb)(void);
  size_t interval;
  unsigned long ts;
} internal_task;

/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static unsigned long _wpa_notify = 0;
static unsigned long _dhcp_notify = 0;
static internal_task _tasks[]{
    {WPA_UPDATE, []() { network_manager.update_wpa_supplicant(WPA_CFG_PATH); }},
    {DHCP_UPDATE, []() { network_manager.update_dhcpcd(DHCP_CFG_PATH); }},
};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
void init() {
  for (auto &task : _tasks) {
    if (task.interval == 0) {
      task.interval = 1000;
    }
  }
}

void loop() {
  for (auto &task : _tasks) {
    if (task.ts != 0 && millis() - task.ts > task.interval) {
      task.ts = 0;
      task.cb();
    }
  }
}

void notify(NOTIFY_INDEX index) {
  for (auto &task : _tasks) {
    if (task.index == index) {
      task.ts = millis();
    }
  }
}

}; // namespace SYSTEM