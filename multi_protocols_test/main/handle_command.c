#include <stdint.h>
#include <math.h>
#include <string.h>
#include "esp_system.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "handle_command.h"
/******************************************************************************/

static gpio_num_t i2c_gpio_sda = 18;
static gpio_num_t i2c_gpio_scl = 19;
static uint32_t i2c_frequency = 100000;
static i2c_port_t i2c_port = I2C_NUM_0;
/******************************************************************************/

uint32_t dice_roll(void)
{
    uint32_t random_num = esp_random();
    uint32_t pos_random_num = abs( random_num );
    uint32_t dice_num = ( pos_random_num % 6 ) + 1;
    return dice_num;
}
/******************************************************************************/

static esp_err_t i2c_master_driver_initialize(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_gpio_sda,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = i2c_gpio_scl,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_frequency
    };
    return i2c_param_config(i2c_port, &conf);
}
/******************************************************************************/

void i2c_scan(char *line_buffer)
{
    uint8_t address[128];
    int qty = 0;
    char buf1[400];
    ESP_ERROR_CHECK(i2c_driver_install(i2c_port, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, 
                                        I2C_MASTER_TX_BUF_DISABLE, 0));
    ESP_ERROR_CHECK(i2c_master_driver_initialize());
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            fflush(stdout);
            address[qty] = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            ESP_ERROR_CHECK(i2c_master_start(cmd));
            ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (address[qty] << 1) | WRITE_BIT, ACK_CHECK_EN));
            ESP_ERROR_CHECK(i2c_master_stop(cmd));
            esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 50 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            if (ret == ESP_OK) {
                printf("%02x ", address[qty++]);
            } else if (ret == ESP_ERR_TIMEOUT) {
                printf("UU ");
            } else {
                printf("-- ");
            }
        }
        printf("\r\n");
    }
    ESP_ERROR_CHECK(i2c_driver_delete(i2c_port));
    sprintf(line_buffer, "%d ", qty);
    if(qty > 0) {
        for (int k = 0; k < qty; k++) {
            sprintf(buf1, "%02x ", address[k]);
            strcat(line_buffer, buf1);
        }
    }
    strcat(line_buffer, "\n");
}
/******************************************************************************/

static char *get_auth_mode_name(wifi_auth_mode_t auth_mode)
{
  char *names[] = {"OPEN", "WEP", "WPA PSK", "WPA2 PSK", "WPA WPA2 PSK", "MAX"};
  return names[auth_mode];
}
/******************************************************************************/

void wifi_scan(char *line_buffer)
{
    char buf1[40];
    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = false
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    wifi_ap_record_t wifi_records[MAX_AP];
    uint16_t max_records = MAX_AP;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&max_records, wifi_records));
    printf("Found %d access points:\n", max_records);
    printf("\n");
    printf("               SSID              | Channel | RSSI |   Auth Mode \n");
    printf("----------------------------------------------------------------\n");
    for (int i = 0; i < max_records; i++) {
        printf("%32s | %7d | %4d | %12s\n", (char *)wifi_records[i].ssid, wifi_records[i].primary, 
                wifi_records[i].rssi, get_auth_mode_name(wifi_records[i].authmode));
        sprintf(buf1, "\"%s\" ", (char *)wifi_records[i].ssid);
        strcat(line_buffer, buf1);
    }
    strcat(line_buffer, "\n");
    printf("----------------------------------------------------------------\n");
}
/******************************************************************************/

void wifi_connect()
{
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_connect());
}
/******************************************************************************/