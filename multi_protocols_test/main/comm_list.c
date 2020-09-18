#include <stdint.h>
#include "comm_list.h"

const char cCommandList[][15] =
{
  "roll dice", // 0
  "i2c scan", // 1
  "wi-fi scan", // 2
  "connect", // 3
  "update" // 4
};

uint8_t ucCommandListSize(void)
{
  return ((sizeof(cCommandList)-1)/(sizeof(cCommandList[0])-1));
}