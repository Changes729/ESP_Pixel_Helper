/** See a brief introduction (right-hand button) */
#include"encoder.h"
/* Private include -----------------------------------------------------------*/
/* Private namespace ---------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ENCODER_GARY_LIMIT 0x03

/* Private typedef -----------------------------------------------------------*/
/* Private template ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** use 'volatile' let compiler do not optimize. */
static uint8_t _PIN_A = -1, _PIN_B = -1;
volatile static long _encoder_count = 0;
volatile static uint8_t _last_gary_code = 0;

/* Private class -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static inline uint8_t _get_encoder_gary_code();
static int _gary_code_to_int(uint8_t gary);
static void IRAM_ATTR _on_encoder_interrupt();

/* Private function ----------------------------------------------------------*/
int encoder_init(uint8_t int_a, uint8_t int_b)
{
  if (_PIN_A == int_a && _PIN_B == int_b)
  {
    return 0;
  }

  if (_PIN_A != uint8_t(-1) && _PIN_B != uint8_t(-1))
  {
    // already init.
    return -1;
  }

  _PIN_A = int_a;
  _PIN_B = int_b;

  // use pull up to enhance drive power.
  pinMode(_PIN_A, INPUT_PULLUP);
  pinMode(_PIN_B, INPUT_PULLUP);

  _last_gary_code = _get_encoder_gary_code();

  attachInterrupt(digitalPinToInterrupt(_PIN_A), _on_encoder_interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(_PIN_B), _on_encoder_interrupt, CHANGE);

  return 0;
}

long get_encoder_count(bool clean_up)
{
  long count = _encoder_count;
  if (clean_up)
  {
    _encoder_count = 0;
  }
  return count;
}

static inline uint8_t _get_encoder_gary_code()
{
  return (digitalRead(_PIN_A) << 1 | digitalRead(_PIN_B));
}

static int _gary_code_to_int(uint8_t gary)
{
  int map[]{0, 1, 3, 2};
  return map[gary & ENCODER_GARY_LIMIT];
}

/**
 * @brief gary code.
 *
 * @see: https://img.alicdn.com/imgextra/i3/3339008665/O1CN017wXw5w2DscW48shUF_!!3339008665.jpg
 * @see: https://item.taobao.com/item.htm?id=620356673590&ns=1&abbucket=1#detail
 * @see: https://github.com/PaulStoffregen/Encoder/blob/master/Encoder.h#L162
 *
 * A'B'  00   01   10   11
 * AB  o-------------------o
 * 00  |  X |  1 | -1 |  X |
 * 01  | -1 |  X |  X |  1 |
 * 10  |  1 |  X |  X | -1 |
 * 11  |  X | -1 |  1 |  X |
 *     o-------------------o
 *
 *     new   new   old   old
 *     pin2  pin1  pin2  pin1  Result
 *     ----  ----  ----  ----  ------
 *     0     0     0     0     no movement
 *     0     0     0     1     +1
 *     0     0     1     0     -1
 *     0     0     1     1     +2  (assume pin1 edges only)
 *     0     1     0     0     -1
 *     0     1     0     1     no movement
 *     0     1     1     0     -2  (assume pin1 edges only)
 *     0     1     1     1     +1
 *     1     0     0     0     +1
 *     1     0     0     1     -2  (assume pin1 edges only)
 *     1     0     1     0     no movement
 *     1     0     1     1     -1
 *     1     1     0     0     +2  (assume pin1 edges only)
 *     1     1     0     1     -1
 *     1     1     1     0     +1
 *     1     1     1     1     no movement
 */
static void IRAM_ATTR _on_encoder_interrupt()
{
  uint8_t curr_gary_code = _get_encoder_gary_code();
  uint8_t index = ((_last_gary_code << 2) | (curr_gary_code));

  switch (index)
  {
  case 0b0001:
  case 0b0111:
  case 0b1000:
  case 0b1110:
    _encoder_count = _encoder_count + 1;
    break;
  case 0b0010:
  case 0b0100:
  case 0b1011:
  case 0b1101:
    _encoder_count = _encoder_count - 1;
    break;

  default:
    break;
  }

  _last_gary_code = curr_gary_code;
}