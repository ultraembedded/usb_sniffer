#ifndef __USB_HELPERS_H__
#define __USB_HELPERS_H__

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

uint8_t  usb_get_pid(uint32_t value);
uint16_t usb_get_cycle_delta(uint32_t value);
int      usb_get_rst_state(uint32_t value);
uint8_t  usb_get_token_device(uint32_t value);
uint8_t  usb_get_token_endpoint(uint32_t value);
uint8_t  usb_get_token_crc5(uint32_t value);
uint16_t usb_get_data_length(uint32_t value);
uint16_t usb_get_sof_frame(uint32_t value);
uint8_t  usb_get_sof_crc5(uint32_t value);

char*    usb_get_pid_str(uint8_t pid);

#ifdef __cplusplus
}
#endif

#endif
