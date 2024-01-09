/** See a brief introduction (right-hand button) */
#include "net_common.h"
/* Private include -----------------------------------------------------------*/
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/

net_resolver::net_resolver(char *buff, size_t buf_size)
    : _buf{buff}, _interface{nullptr}, _buf_size{buf_size}, _offset{0} {}

void net_resolver::resolve(config_cb_t callback) {
  assert(_offset == 0 && callback != nullptr);
  if (_offset != 0 || callback == nullptr) {
    /** resolver has resolve before. */
    return;
  }

  char *line = _read_line();
  assert(line != nullptr);
  while (line[0] != '\0') {
    _resolve_option(callback, _strip(line));
    line = _read_line();
  }
}

char *net_resolver::_read_line() {
  char *index = _buf + _offset;
  while (_buf[_offset] != '\0') {
    auto index = _offset;
    _offset = _offset + 1;

    if (_buf[index] == '\n') {
      _buf[index] = '\0';
      break;
    }
  }
  return index;
}

char *net_resolver::_read_arg(char *str) {
  assert(str != nullptr);

  while (isgraph(*str)) {
    str = str + 1;
  }

  char *index = nullptr;
  if (*str != '\0') {
    *str = '\0';
    index = str + 1;
  }
  return index;
}

char *net_resolver::_strip(char *line) {
  char *begin = line;
  size_t end_index = strlen(line);
  assert(end_index > 0);

  while (!isgraph(begin[end_index])) {
    end_index--;
  }
  begin[end_index + 1] = '\0';

  while (!isgraph(*begin)) {
    begin++;
  }

  return begin;
}

bool net_resolver::_resolve_option(config_cb_t callback, char *raw_opt) {
  char *option = raw_opt;
  char *arg = _read_arg(option);

  if (strcmp(option, "interface") == 0) {
    if (arg == nullptr) {
      /** interface should have an argument. */
      assert(false);
    } else {
      _interface = arg;
    }
  } else {
    callback(_interface, option, arg);
  }

  return true;
}