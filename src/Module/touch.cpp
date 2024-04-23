/** See a brief introduction (right-hand button) */
#include "touch.h"
/* Private include -----------------------------------------------------------*/
#include <Adafruit_MPR121.h>
#include <Wire.h>

#include "config.h"
#include "system_operation.h"

/* Private namespace ---------------------------------------------------------*/
namespace Touch {

/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Adafruit_MPR121 cap;
static bool _started = false;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
void touch_init() {
  bool success = true;
  Wire.setPins(MPR121_SDA, MPR121_SCL);
  if (!cap.begin(0x5A, &Wire)) {
    log_e("MPR121 not found, check wiring?");
    success = false;
  } else {
    cap.setThresholds(60, 20);
    cap.writeRegister(MPR121_MHDR, 0x01);
    cap.writeRegister(MPR121_NHDR, 0x01);
    cap.writeRegister(MPR121_NCLR, 0x02);
    cap.writeRegister(MPR121_FDLR, 0x00);

    cap.writeRegister(MPR121_MHDF, 0x01);
    cap.writeRegister(MPR121_NHDF, 0x01);
    cap.writeRegister(MPR121_NCLF, 0xFF);

    /** CDC(5:0) 1uA~63uA */
    cap.writeRegister(MPR121_CONFIG1, 0b00001010);
    /** CDT(7:5) 0.5us~32us */
    cap.writeRegister(MPR121_CONFIG2, 0b11000000);
  }

  _started = success;
}

void set_thresholds(uint8_t touch, uint8_t release) {
  cap.setThresholds(touch, release);
}

uint16_t check_touch_state() { return cap.touched(); }

bool is_touch_started() { return _started; }

void set_debug_info(char *buffer, size_t size) {
  snprintf(buffer, size,
           "%i: %u\n"
           "%i: %u\n",
           1, cap.filteredData(TOUCH_LEFT), 2, cap.filteredData(TOUCH_RIGHT));
}

}; // namespace Touch
