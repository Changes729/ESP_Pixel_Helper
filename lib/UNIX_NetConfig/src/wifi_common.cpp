/** See a brief introduction (right-hand button) */
#include "wifi_common.h"
/* Private include -----------------------------------------------------------*/
#include <assert.h>
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
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
size_t wifi_resolver::count_settings(const char *config_raw) {
  size_t count = 0;

  while ((config_raw = strstr(config_raw, "network={")) != nullptr &&
         (config_raw = strstr(config_raw, "}")) != nullptr) {
    count++;
  }

  return count;
}

wifi_resolver::wifi_resolver(char *buff, size_t buf_size)
    : _buf{buff}, _settings{nullptr}, _buf_size{buf_size}, _offset{0} {}

void wifi_resolver::resolve(config_cb_t callback) {
  assert(_offset == 0 && callback != nullptr);
  if (_offset != 0 || callback == nullptr) {
    /** resolver has resolve before. */
    return;
  }

  char *line = _read_line();
  assert(line != nullptr);
  while (line[0] != '\0') {
    _resolve_settings(callback, _strip(line));
    line = _read_line();
  }
}

char *wifi_resolver::_read_line() {
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

char *wifi_resolver::_read_arg(char *str) {
  assert(str != nullptr);

  while (str[0] != '\0') {
    if (str[0] == '=') {
      str[0] = '\0';
      break;
    }
    str = str + 1;
  }

  return str + 1;
}

char *wifi_resolver::_strip(char *line) {
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

bool wifi_resolver::_resolve_settings(config_cb_t callback, char *raw_opt) {
  char *option = raw_opt;
  char *arg = _read_arg(option);

  if (_settings == nullptr) {
    if (arg[0] == '{') {
      _settings = option;
      callback(_settings, option, arg);
    }
  } else if (option[0] == '}') {
    _settings = nullptr;
  } else {
    callback(_settings, option, arg);
  }

  return true;
}