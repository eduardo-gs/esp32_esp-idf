#ifndef __HANDLE_COMMAND_H__
#define __HANDLE_COMMAND_H__

#include <stdint.h>

uint32_t dice_roll(void);
void do_i2cdetect_cmd(char *line_buffer);
void wifi_scan(void);

#endif