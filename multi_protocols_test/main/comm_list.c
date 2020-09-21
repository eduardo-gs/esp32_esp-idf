#include <stdint.h>
#include "comm_list.h"
/******************************************************************************/

const char comm_list[][15] =
{
  "dice roll",  // 0
  "i2c detect", // 1
  "wi-fi scan", // 2
  "connect",    // 3
  "update"      // 4
};
/******************************************************************************/

uint8_t comm_list_size(void)
{
  return ((sizeof(comm_list)-1)/(sizeof(comm_list[0])-1));
}
/******************************************************************************/
