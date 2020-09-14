#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"

#include <stddef.h>

extern int btstack_main(int argc, const char * argv[]);

void app_main(void)
{
  // Configure BTstack for ESP32 VHCI Controller
  btstack_init();

  // Setup example
  btstack_main(0, NULL);

  // Enter run loop (forever)
  btstack_run_loop_execute();

  return 0;
}
