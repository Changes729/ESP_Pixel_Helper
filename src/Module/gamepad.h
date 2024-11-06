/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GAMEPAD_H
#define GAMEPAD_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include "Language/instance.h"
#include <Joystick_ESP32S2.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
class GamePad : public Instance<GamePad>, public Joystick_ {
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
    int8_t x;
    int8_t y;
  } _stick_left;

  struct {
    int8_t x;
    int8_t y;
  } _stick_right;
};
#endif /* GAMEPAD_H */
