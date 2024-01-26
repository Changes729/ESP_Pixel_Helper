/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INSTANCE_H
#define INSTANCE_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <assert.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
template <class C> class Instance {
public:
  static C &instance() { return *_instance; }
  template <typename... __arg_pkg> static C *create(__arg_pkg... args) {
    assert(_instance == nullptr);
    _instance = new C(args...);
    assert(_instance != nullptr);
    return _instance;
  }
  static void destroy() {
    delete _instance;
    _instance = nullptr;
  }

private:
  static C *_instance;
};

template <class C> C *Instance<C>::_instance = nullptr;

#endif /* INSTANCE_H */
