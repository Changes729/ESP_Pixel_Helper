/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GAMEPAD_H
#define GAMEPAD_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "Language/instance.h"
#include "USBHIDGamepad.h"

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class GamePad : public Instance<GamePad>, public USBHIDGamepad {
private:
  friend Instance<GamePad>;
  GamePad();

public:
  void update();

private:
  struct {
    bool a;
    bool b;
    bool x;
    bool y;

    bool center;
  } _key;

  struct {
    bool up;
    bool dw;
    bool lf;
    bool ri;
  } _hat;

  struct {
    uint8_t x;
    uint8_t y;
  } _stick_left;

  struct {
    uint8_t x;
    uint8_t y;
  } _stick_right;
};
#endif /* GAMEPAD_H */
