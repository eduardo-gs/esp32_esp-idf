#ifndef __HANDLE_COMMAND_H__
#define __HANDLE_COMMAND_H__
/******************************************************************************/

#include <stdint.h>
/******************************************************************************/

#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define WRITE_BIT I2C_MASTER_WRITE
#define ACK_CHECK_EN 0x1
/******************************************************************************/

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define MAX_AP 5
/******************************************************************************/

uint32_t dice_roll(void);
void i2c_scan(char *line_buffer);
void wifi_scan(char *line_buffer);
void wifi_connect(void);
/******************************************************************************/

#endif