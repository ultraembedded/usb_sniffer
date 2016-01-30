//-----------------------------------------------------------------
//                       USB Sniffer
//                           V0.1
//                     Ultra-Embedded.com
//                       Copyright 2015
//
//               Email: admin@ultra-embedded.com
//
//                       License: LGPL
//-----------------------------------------------------------------
//
// Copyright (C) 2011 - 2013 Ultra-Embedded.com
//
// This source file may be used and distributed without         
// restriction provided that this copyright statement is not    
// removed from the file and that any derivative work contains  
// the original copyright notice and the associated disclaimer. 
//
// This source file is free software; you can redistribute it   
// and/or modify it under the terms of the GNU Lesser General   
// Public License as published by the Free Software Foundation; 
// either version 2.1 of the License, or (at your option) any   
// later version.
//
// This source is distributed in the hope that it will be       
// useful, but WITHOUT ANY WARRANTY; without even the implied   
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      
// PURPOSE.  See the GNU Lesser General Public License for more 
// details.
//
// You should have received a copy of the GNU Lesser General    
// Public License along with this source; if not, write to the 
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA  02111-1307  USA
//-----------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "usb_defs.h"
#include "usb_helpers.h"
#include "log_file_usb.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define PAYLOAD_TYPE_SHIFT          6
#define PAYLOAD_TYPE_MASK           0x3
    #define PAYLOAD_TYPE_NONE           0x0
    #define PAYLOAD_TYPE_EVENT          0x1
    #define PAYLOAD_TYPE_DATA           0x2
    #define PAYLOAD_TYPE_RXCMD          0x3

#define PACKET_LEN_SHIFT            4
#define PACKET_LEN_MASK             0x3

#define TIME_INC_SHIFT              0
#define TIME_INC_MASK               0xF

#define LINESTATE_IDLE             0x1
#define LINESTATE_SE0              0x0

#define TICKS_PER_HS_UFRAME        7500
#define TICKS_PER_FSLS_FRAME       60000

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static FILE *_file;
static uint32_t _last_tic = 0;
static int _in_rst = -1;

//-----------------------------------------------------------------
// usb_file_add_time: Add large time offset (>= 4096 ticks)
//-----------------------------------------------------------------
static int usb_file_add_time(uint32_t tic_inc)
{
    uint8_t hdr = 0;
    uint8_t packet[4];

    hdr |= ((tic_inc & TIME_INC_MASK) << TIME_INC_SHIFT);
    hdr |= (PAYLOAD_TYPE_NONE << PAYLOAD_TYPE_SHIFT);
    hdr |= (3 << PACKET_LEN_SHIFT);

    packet[1] = hdr;
    packet[0] = (tic_inc >> 4);
    packet[3] = (tic_inc >> 12);
    packet[2] = (tic_inc >> 20);

    fwrite(packet, 1, 4, _file);

    return 0;   
}
//-----------------------------------------------------------------
// usb_file_add_rxcmd: Add rx status message entry to the log
//-----------------------------------------------------------------
static int usb_file_add_rxcmd(uint32_t tic_inc, uint8_t linestate, int rx_active, int rx_error, int hostdisconnect)
{
    #define NORMAL_INC_MASK 0xFFF
    #define NORMAL_INC_MAX  (4096 - 1)

    uint8_t hdr = 0;
    uint8_t hdr_payload = (tic_inc >> 4);
    uint8_t packet[4];
    uint8_t rx_event = 0;

    if (tic_inc > NORMAL_INC_MAX)
    {
        // Add larger time value into stream
        usb_file_add_time(tic_inc & ~NORMAL_INC_MASK);
    }

    hdr |= ((tic_inc & TIME_INC_MASK) << TIME_INC_SHIFT);
    hdr |= (PAYLOAD_TYPE_RXCMD << PAYLOAD_TYPE_SHIFT);
    hdr |= (1 << PACKET_LEN_SHIFT);

    if (rx_active == 0 && rx_error == 0)
        rx_event = 0x0;
    else if (rx_active == 1 && rx_error == 0)
        rx_event = 0x1;
    else if (rx_active == 1 && rx_error == 1)
        rx_event = 0x3;
    else
        rx_event = 0x2;

    packet[1] = hdr;
    packet[0] = hdr_payload;
    packet[3] = linestate | rx_event << 4;
    packet[2] = 0x00;

    fwrite(packet, 1, 4, _file);

    _last_tic += tic_inc;

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_data_byte: Add received data byte to log
//-----------------------------------------------------------------
static int usb_file_add_data_byte(uint32_t tic_inc, uint8_t data)
{
    uint8_t hdr = 0;
    uint8_t hdr_payload = (tic_inc >> 4);

    hdr |= ((tic_inc & TIME_INC_MASK) << TIME_INC_SHIFT);
    hdr |= (PAYLOAD_TYPE_DATA << PAYLOAD_TYPE_SHIFT);
    hdr |= (1 << PACKET_LEN_SHIFT);

    uint8_t packet[4];

    packet[1] = hdr;
    packet[0] = hdr_payload;
    packet[3] = data;
    packet[2] = 0x00;

    fwrite(packet, 1, 4, _file);

    _last_tic += tic_inc;

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_sof: Add start of frame token to log
//-----------------------------------------------------------------
int usb_file_add_sof(uint32_t value, int is_hs)
{
    int i;
    uint16_t frame_num = usb_get_sof_frame(value);

    uint8_t sof_data[3];
    uint8_t crc5 = usb_get_sof_crc5(value);

    sof_data[0] = PID_SOF;
    sof_data[1] = frame_num & 0xFF;
    sof_data[2] = (frame_num >> 8) & 0x7;
    sof_data[2]|= (crc5 << 3);

    // Work out delta between last message and next SOF boundary
    int tics_per_frame = is_hs ? TICKS_PER_HS_UFRAME : TICKS_PER_FSLS_FRAME;
    int delta = tics_per_frame - _last_tic;
    if (delta <= 0)
        delta = 1;    

    usb_file_add_rxcmd(delta, LINESTATE_IDLE, 1, 0, 0);
    for (i=0;i<sizeof(sof_data);i++)
        usb_file_add_data_byte(1, sof_data[i]);
    usb_file_add_rxcmd(1, LINESTATE_IDLE, 0, 0, 0);

    _last_tic = 0;

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_rst: Add reset event to the log
//-----------------------------------------------------------------
int usb_file_add_rst(uint32_t value, int is_hs)
{
    int      in_rst = usb_get_rst_state(value);
    uint16_t cycle  = usb_get_cycle_delta(value);

    if (in_rst != _in_rst)
    {
        // TODO: Add support for chirp detection
        int reset_time = in_rst ? cycle : (TICKS_PER_FSLS_FRAME * 10);
        usb_file_add_rxcmd(reset_time, in_rst ? LINESTATE_SE0 : LINESTATE_IDLE, 0, 0, 0);

        _last_tic = 0;

        _in_rst = in_rst;
    }

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_token: Add token (IN, OUT, SETUP, PING)
//-----------------------------------------------------------------
int usb_file_add_token(uint32_t value)
{
    int i;
    uint8_t token[3];

    uint8_t pid          = usb_get_pid(value);
    uint8_t device       = usb_get_token_device(value);
    uint8_t endpoint     = usb_get_token_endpoint(value);
    uint16_t delta_time  = usb_get_cycle_delta(value);
    uint8_t crc5         = usb_get_token_crc5(value);

    token[0] = pid;
    token[1] = device & 0x7F;
    token[1]|= (endpoint << 7) & 0x80;
    token[2] = (endpoint >> 1) & 0x7;
    token[2]|= (crc5 << 3);

    usb_file_add_rxcmd(delta_time, LINESTATE_IDLE, 1, 0, 0);

    for (i=0;i<sizeof(token);i++)
        usb_file_add_data_byte(1, token[i]);

    usb_file_add_rxcmd(1, LINESTATE_IDLE, 0, 0, 0);

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_handshake: Add handshake (ACK, NAK, NYET)
//-----------------------------------------------------------------
int usb_file_add_handshake(uint32_t value)
{
    uint8_t pid          = usb_get_pid(value);
    uint16_t delta_time  = usb_get_cycle_delta(value);

    usb_file_add_rxcmd(delta_time, LINESTATE_IDLE, 1, 0, 0);
    usb_file_add_data_byte(1, pid);
    usb_file_add_rxcmd(1, LINESTATE_IDLE, 0, 0, 0);

    return 0;
}
//-----------------------------------------------------------------
// usb_file_add_data: Add data packet to log
//-----------------------------------------------------------------
int usb_file_add_data(uint32_t value, uint8_t *data, int length)
{
    int i;
    uint8_t pid         = usb_get_pid(value);
    uint16_t delta_time = usb_get_cycle_delta(value);

    usb_file_add_rxcmd(delta_time, LINESTATE_IDLE, 1, 0, 0);
    usb_file_add_data_byte(1, pid);
    for (i=0;i<length;i++)
        usb_file_add_data_byte(1, data[i]);
    usb_file_add_rxcmd(1, LINESTATE_IDLE, 0, 0, 0);

    return 0;
}
//-----------------------------------------------------------------
// usb_file_create: Create & open empty log file
//-----------------------------------------------------------------
int usb_file_create(const char *filename)
{
    _file = fopen(filename, "wb");   
    return _file == NULL ? -1 : 0;
}
//-----------------------------------------------------------------
// usb_file_close: Close open file handle
//-----------------------------------------------------------------
int usb_file_close(void)
{
    if (_file != NULL)
        fclose(_file);

    _file = NULL;

    return 0;
}
