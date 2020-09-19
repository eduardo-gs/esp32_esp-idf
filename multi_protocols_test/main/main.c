#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"
#include "esp_wifi.h"

#include <stddef.h>

extern int btstack_main(int argc, const char * argv[]);

void init_wifi()
{
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
  assert(sta_netif);
}

void app_main(void)
{
  // Configure BTstack for ESP32 VHCI Controller
  btstack_init();

  // Initialize basic wi-fi settings
  init_wifi();

  // Setup example
  btstack_main(0, NULL);

  // Enter run loop (forever)
  btstack_run_loop_execute();

  return 0;
}
