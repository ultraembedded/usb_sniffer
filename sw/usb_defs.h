#ifndef __USB_DEFS_H__
#define __USB_DEFS_H__

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
// Tokens
#define PID_OUT                    0xE1
#define PID_IN                     0x69
#define PID_SOF                    0xA5
#define PID_SETUP                  0x2D
#define PID_PING                   0xB4

// Data
#define PID_DATA0                  0xC3
#define PID_DATA1                  0x4B
#define PID_DATA2                  0x87
#define PID_MDATA                  0x0F

// Handshake
#define PID_ACK                    0xD2
#define PID_NAK                    0x5A
#define PID_STALL                  0x1E
#define PID_NYET                   0x96

// Special
#define PID_PRE                    0x3C
#define PID_ERR                    0x3C
#define PID_SPLIT                  0x78

// Rounded up...
#define MAX_PACKET_SIZE 		   2048

#endif
