/** See a brief introduction (right-hand button) */
#include "gamepad.h"
/* Private include -----------------------------------------------------------*/
#include <Arduino.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_A 5
#define KEY_B 6
#define KEY_X 7
#define KEY_Y 8
#define KEY_UP 9
#define KEY_DW 10
#define KEY_LF 11
#define KEY_RI 12
#define KEY_CENTER 33

#define isPress(state) (state == HIGH)

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
GamePad::GamePad() : Joystick_{} {
  for (auto pin : {
           KEY_A,
           KEY_B,
           KEY_X,
           KEY_Y,
           KEY_UP,
           KEY_DW,
           KEY_LF,
           KEY_RI,
       }) {
    pinMode(pin, INPUT_PULLUP);
  }

  setXAxisRange(-127, 127);
  setYAxisRange(-127, 127);
  setZAxisRange(-127, 127);

  begin();
}

void GamePad::update() {
  /** key part ******************************************************/
  const struct {
    bool &_data;
    int pin;
    uint8_t button;
  } PIN_MAP[]{
      {._data = _key.a, .pin = KEY_A, .button = KEY_A},
      {._data = _key.b, .pin = KEY_B, .button = KEY_B},
      {._data = _key.x, .pin = KEY_X, .button = KEY_X},
      {._data = _key.y, .pin = KEY_Y, .button = KEY_Y},
      {._data = _key.center, .pin = KEY_CENTER, .button = KEY_CENTER},
  };

  for (auto &map : PIN_MAP) {
    bool state = digitalRead(map.pin);
    if (state != map._data) {
      isPress(state) ? pressButton(map.button) : releaseButton(map.button);
    }
    map._data = state;
  }

  /** stick part ****************************************************/
  _stick_left.x = (digitalRead(KEY_UP) * 127 + digitalRead(KEY_DW) * -127);
  _stick_left.y = (digitalRead(KEY_LF) * 127 + digitalRead(KEY_RI) * -127);
  setXAxis(_stick_left.x);
  setYAxis(_stick_left.y);

  setZAxis(map(analogRead(DAC2), 0, 0x1FFF, -127, 127));
}
