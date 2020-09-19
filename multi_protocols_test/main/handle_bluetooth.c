#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "btstack.h"
#include "comm_list.h"
#include "handle_command.h"

#define RFCOMM_SERVER_CHANNEL 1

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);

static uint16_t rfcomm_channel_id;
static uint8_t  spp_service_buffer[150];
static btstack_packet_callback_registration_t hci_event_callback_registration;
static uint16_t size_send_packet;
static char line_buffer[400];
static uint32_t dice_face;

static void spp_service_setup(void)
{
    // register for HCI events
    hci_event_callback_registration.callback = &packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    rfcomm_init();
    rfcomm_register_service(packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff);  // reserved channel, mtu limited by l2cap

    // init SDP, create record for SPP and register with SDP
    sdp_init();
    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, 0x10001, RFCOMM_SERVER_CHANNEL, "SPP");
    sdp_register_service(spp_service_buffer);
    printf("SDP service record size: %u\n", de_get_len(spp_service_buffer));
}

static void packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    UNUSED(channel);

/* LISTING_PAUSE */ 
    bd_addr_t event_addr;
    uint8_t   rfcomm_channel_nr;
    uint16_t  mtu;
    uint8_t ucCount = 0, ucCheck = 0, ucCommandSelect = 0;

    switch (packet_type)
    {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet))
            {
/* LISTING_RESUME */ 
                case HCI_EVENT_PIN_CODE_REQUEST:
                    // inform about pin code request
                    printf("Pin code request - using '0000'\n");
                    hci_event_pin_code_request_get_bd_addr(packet, event_addr);
                    gap_pin_code_response(event_addr, "0000");
                    break;

                case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                    // ssp: inform about user confirmation request
                    printf("SSP User Confirmation Request with numeric value '%06"PRIu32"'\n", little_endian_read_32(packet, 8));
                    printf("SSP User Confirmation Auto accept\n");
                    break;

                case RFCOMM_EVENT_INCOMING_CONNECTION:
                    // data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
                    rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr); 
                    rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
                    rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
                    printf("RFCOMM channel %u requested for %s\n", rfcomm_channel_nr, bd_addr_to_str(event_addr));
                    rfcomm_accept_connection(rfcomm_channel_id);
                    break;
               
                case RFCOMM_EVENT_CHANNEL_OPENED:
                    // data: event(8), len(8), status (8), address (48), server channel(8), rfcomm_cid(16), max frame size(16)
                    if (rfcomm_event_channel_opened_get_status(packet))
                    {
                        printf("RFCOMM channel open failed, status %u\n", rfcomm_event_channel_opened_get_status(packet));
                    }
                    else
                    {
                        rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                        mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
                        printf("RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n", rfcomm_channel_id, mtu);
                    }
                    break;
                case RFCOMM_EVENT_CAN_SEND_NOW:
                    rfcomm_send(rfcomm_channel_id, (uint8_t *)line_buffer, size_send_packet);
                    break;

/* LISTING_PAUSE */                 
                case RFCOMM_EVENT_CHANNEL_CLOSED:
                    printf("RFCOMM channel closed\n");
                    rfcomm_channel_id = 0;
                    break;
                
                default:
                    break;
            }
            break;

        case RFCOMM_DATA_PACKET:
            printf("RCV: ");
            for (ucCount=0;ucCount<size;ucCount++)
            {
                putchar(packet[ucCount]);
            }
            printf("\n");
            
            for( ucCount = 0; ucCount < ucCommandListSize(); ucCount++ )
            {
                ucCheck = strncmp( ( char * ) packet, cCommandList[ ucCount ], strlen( cCommandList[ ucCount ] ) );
                if(ucCheck == 0)
                {
                    ucCommandSelect = ucCount;
                    break;
                }
                else
                {
                    // Number out of the command list range.
                    ucCommandSelect = 100;
                }
            }     
            switch (ucCommandSelect)
            {
                case 0:
                    dice_face = dice_roll();
                    printf("Dice Number: %d\n", dice_face);
                    sprintf(line_buffer, "%d\n", dice_face);
                    break;
                case 1:
                    do_i2cdetect_cmd(&line_buffer);
                    // strcpy(line_buffer, "2\n");
                    break;
                case 2:
                    wifi_scan();
                    strcpy(line_buffer, "3\n");
                    break;
                case 3:
                    strcpy(line_buffer, "4\n");
                    break;
                case 4:
                    strcpy(line_buffer, "5\n");
                    break;
                default:
                    strcpy(line_buffer, "ERROR\n");
                    break;
            }
            size_send_packet = strlen(line_buffer);
            rfcomm_request_can_send_now_event(rfcomm_channel_id);
            break;

        default:
            break;
    }
/* LISTING_RESUME */ 
}
/* LISTING_END */

int btstack_main(int argc, const char * argv[]);
int btstack_main(int argc, const char * argv[])
{
    (void)argc;
    (void)argv;

    spp_service_setup();

    gap_discoverable_control(1);
    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
    gap_set_local_name("ESP32");

    // turn on!
    hci_power_control(HCI_POWER_ON);
    
    return 0;
}
