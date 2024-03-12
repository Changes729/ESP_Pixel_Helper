/** See a brief introduction (right-hand button) */
#include "nfc.h"
/* Private include -----------------------------------------------------------*/
#include "config.h"

#include <PN5180.h>
#include <PN5180ISO15693.h>

/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);
static uint8_t _uid[8];

static uint8_t standardpassword[] = {0x0F, 0x0F, 0x0F, 0x0F};
static uint8_t password[] = {0x12, 0x34, 0x56, 0x78};

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void _showIRQStatus(uint32_t irqStatus);

/* Private function ----------------------------------------------------------*/
/* Private class function ----------------------------------------------------*/
/**
 * @brief  ...
 * @param  None
 * @retval None
 */
void nfc_init() {
  SPI.begin(PN5180_SCK, PN5180_MISO, PN5180_MOSI);
  nfc.begin();
  nfc.reset();
  nfc.setupRF();
}

void update_nfc() {
  ISO15693ErrorCode rc = nfc.getInventory(_uid);
  if (ISO15693_EC_OK != rc) {
    log_e("Error in getInventory: ");

    uint32_t irqStatus = nfc.getIRQStatus();
    _showIRQStatus(irqStatus);

    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      log_d(F("*** No card detected!"));
    }

    nfc.reset();
    nfc.setupRF();
  }

#if 0
  Serial.println(F("----------------------------------"));
  uint8_t blockSize, numBlocks;
  rc = nfc.getSystemInfo(uid, &blockSize, &numBlocks);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Error in getSystemInfo: "));
    errorFlag = true;
    return;
  }
  Serial.print(F("System Info retrieved: blockSize="));
  Serial.print(blockSize);
  Serial.print(F(", numBlocks="));
  Serial.println(numBlocks);

  Serial.println(F("----------------------------------"));
  uint8_t readBuffer[blockSize];
  for (int no = 0; no < numBlocks; no++) {
    rc = nfc.readSingleBlock(uid, no, readBuffer, blockSize);
    if (ISO15693_EC_OK != rc) {
      Serial.print(F("Error in readSingleBlock #"));
      Serial.print(no);
      Serial.print(": ");
      errorFlag = true;
      return;
    }
    Serial.print(F("Read block #"));
    Serial.print(no);
    Serial.print(": ");
    for (int i = 0; i < blockSize; i++) {
      if (readBuffer[i] < 16)
        Serial.print("0");
      Serial.print(readBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" ");
    for (int i = 0; i < blockSize; i++) {
      if (isprint(readBuffer[i])) {
        Serial.print((char)readBuffer[i]);
      } else
        Serial.print(".");
    }
    Serial.println();
  }
#endif
}

bool has_uid(uint8_t *buffer, size_t size) {
  bool has_uid_ = false;
  for (auto byte : _uid) {
    if (byte != 0x00) {
      has_uid_ = true;
      break;
    }
  }

  if (has_uid_ && buffer != nullptr) {
    memcpy(buffer, _uid, min(size, sizeof(_uid)));
  }

  return has_uid_;
}

bool has_uid(String &out, const char seq) {
  bool has_uid_ = has_uid();
  char buff[3]{};
  if (has_uid_) {
    out.clear();
    for (int i = 0; i < sizeof(_uid); ++i) {
      /** big-endian */
      snprintf(buff, sizeof(buff), "%02X", _uid[i]);
      out += buff;
      if (isalpha(seq) && i != 7) {
        out += seq;
      }
    }
  }

  return has_uid_;
}

static void _uid_to_string(String &str) { str.clear(); }

static void _showIRQStatus(uint32_t irqStatus) {
  Serial.print(F("IRQ-Status 0x"));
  Serial.print(irqStatus, HEX);
  Serial.print(": [ ");
  if (irqStatus & (1 << 0))
    Serial.print(F("RQ "));
  if (irqStatus & (1 << 1))
    Serial.print(F("TX "));
  if (irqStatus & (1 << 2))
    Serial.print(F("IDLE "));
  if (irqStatus & (1 << 3))
    Serial.print(F("MODE_DETECTED "));
  if (irqStatus & (1 << 4))
    Serial.print(F("CARD_ACTIVATED "));
  if (irqStatus & (1 << 5))
    Serial.print(F("STATE_CHANGE "));
  if (irqStatus & (1 << 6))
    Serial.print(F("RFOFF_DET "));
  if (irqStatus & (1 << 7))
    Serial.print(F("RFON_DET "));
  if (irqStatus & (1 << 8))
    Serial.print(F("TX_RFOFF "));
  if (irqStatus & (1 << 9))
    Serial.print(F("TX_RFON "));
  if (irqStatus & (1 << 10))
    Serial.print(F("RF_ACTIVE_ERROR "));
  if (irqStatus & (1 << 11))
    Serial.print(F("TIMER0 "));
  if (irqStatus & (1 << 12))
    Serial.print(F("TIMER1 "));
  if (irqStatus & (1 << 13))
    Serial.print(F("TIMER2 "));
  if (irqStatus & (1 << 14))
    Serial.print(F("RX_SOF_DET "));
  if (irqStatus & (1 << 15))
    Serial.print(F("RX_SC_DET "));
  if (irqStatus & (1 << 16))
    Serial.print(F("TEMPSENS_ERROR "));
  if (irqStatus & (1 << 17))
    Serial.print(F("GENERAL_ERROR "));
  if (irqStatus & (1 << 18))
    Serial.print(F("HV_ERROR "));
  if (irqStatus & (1 << 19))
    Serial.print(F("LPCD "));
  Serial.println("]");
}
