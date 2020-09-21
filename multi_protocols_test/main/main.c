#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <stddef.h>
/******************************************************************************/

extern int btstack_main(int argc, const char * argv[]);
extern esp_err_t event_handler(void *ctx, system_event_t *event);
/******************************************************************************/

void wifi_init()
{
  ESP_ERROR_CHECK(nvs_flash_init());
  tcpip_adapter_init();
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
}
/******************************************************************************/

void app_main(void)
{ 
  // Configure BTstack for ESP32 VHCI Controller
  btstack_init();

  // Initialize basic wi-fi settings
  wifi_init();

  // Setup example
  btstack_main(0, NULL);

  // Enter run loop (forever)
  btstack_run_loop_execute();
}
/******************************************************************************/
