/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H
#pragma once
/* Public include ------------------------------------------------------------*/
#include <stddef.h>

/* Public namespace ----------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
class wifi_config {
public:
  wifi_config(const char *ssid = nullptr, const char *passwd = nullptr);
  wifi_config(wifi_config&& right);
  ~wifi_config();

  void set_ssid(const char *ssid);
  void set_passwd(const char *passwd);

  const char *get_ssid() const;
  const char *get_passwd() const;

  bool is_valid() const;
  bool is_ssid_equal(const char* ssid) const;
  void clear();

private:
  char *_ssid;
  char *_passwd;
};

/* Public template -----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public class --------------------------------------------------------------*/
size_t wifi_config_count();
const wifi_config *wifi_config_get(size_t index);
int wifi_config_set(const char *ssid, const char *passwd);
int wifi_config_clear(const char *ssid);

int wifi_config_init(const char *f);
int wifi_config_update(const char *f);
int wifi_config_deinit();

#endif /* WIFI_CONFIG_H */
