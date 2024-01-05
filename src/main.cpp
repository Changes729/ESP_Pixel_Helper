#include "config.h"

#include <Arduino.h>
#include <LittleFS.h>

#include "UNIX_NetConfig.h"

void test_FS() {
  if (LittleFS.exists("/test.txt")) {
    File file = LittleFS.open("/test.txt", "r");
    String data = file.readString();

    Serial.println(data);

    file.close();
  } else {
    Serial.println("/test.txt Not exist");
    File config_file = LittleFS.open("/test.txt", "w");
    config_file.println("Hello world\n");
    config_file.close();
  }

  if (LittleFS.exists("/file1.txt")) {
    File file = LittleFS.open("/file1.txt", "r");
    String data = file.readString();

    Serial.println(data);

    file.close();
  }
}

void setup() {
  /** Params init */
  /** Serial init */
  Serial.begin(115200);

  net_config_init("/dhcpcd.conf");

  /** LittleFS init */
  if (!LittleFS.begin(false, "")) {
    Serial.println("LittleFS mount failed");
    return;
  }

  test_FS();
}

void loop() {

}