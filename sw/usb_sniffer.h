#ifndef __USB_SNIFFER_H__
#define __USB_SNIFFER_H__

//--------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------
typedef enum
{
	USB_SPEED_HS,
	USB_SPEED_FS,
	USB_SPEED_LS
} tUsbSpeed;

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int usb_sniffer_init(void);
int usb_sniffer_close(void);

int usb_sniffer_setup_mem(uint32_t base, uint32_t size);
int usb_sniffer_match_device(int dev, int exclude);
int usb_sniffer_match_endpoint(int ep, int exclude);
int usb_sniffer_drop_sof(int drop_sof);
int usb_sniffer_continuous_mode(int enable);
int usb_sniffer_set_speed(tUsbSpeed speed);
int usb_sniffer_start(void);
int usb_sniffer_stop(void);
int usb_sniffer_triggered(void);
int usb_sniffer_wrapped(void);
int usb_sniffer_overrun(void);
uint32_t usb_sniffer_current(int *overflow);
uint32_t usb_sniffer_base(void);
uint32_t usb_sniffer_end(void);
int usb_sniffer_set_rd_ptr(uint32_t addr);
int usb_sniffer_get_buffer(uint8_t *buffer, int buffer_size);
int usb_sniffer_read_buffer(uint8_t *buffer, uint32_t base, int size);
int usb_sniffer_extract_buffer(FILE *f, uint32_t rd_ptr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif
