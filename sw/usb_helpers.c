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
#include "log_format.h"
#include "usb_helpers.h"

//-----------------------------------------------------------------
// usb_swap8
//-----------------------------------------------------------------
static uint8_t usb_swap8(uint8_t b) 
{
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}
//-----------------------------------------------------------------
// usb_swap5
//-----------------------------------------------------------------
static uint8_t usb_swap5(uint8_t value)
{
    return (
        ((value & 0x01) << 4) |
        ((value & 0x02) << 2) |
         (value & 0x04) |
        ((value & 0x08) >> 2) |
        ((value & 0x10) >> 4)
    );
}
//-----------------------------------------------------------------
// usb_crc5
//-----------------------------------------------------------------
static uint8_t usb_crc5(uint16_t data)
{
    uint32_t  a;
    uint32_t  crc;
    uint32_t  crc_n;
    int bits = 11;

    a   = (usb_swap8(data) << 8) | (usb_swap8((data >> 8) & 0x7));
    crc = (0x1f << 11);
    
    while (bits--) {
        crc_n   = (crc << 1) & 0xffff;
        crc_n  ^= ((a ^ crc) & (1<<15)) ? (0x05 << 11) : 0;
        crc     = crc_n;
        a       = (a << 1) & 0xffff;
    }
    
    crc >>= 11;
    crc ^= 0x1f;

    return (uint8_t)usb_swap5(crc);
}
//-----------------------------------------------------------------
// usb_get_pid
//-----------------------------------------------------------------
uint8_t usb_get_pid(uint32_t value)
{
    uint8_t pid = ((value >> LOG_TOKEN_PID_L) & LOG_TOKEN_PID_MASK);
    pid |= (~(pid << 4)) & 0xF0;
    return pid;
}
//-----------------------------------------------------------------
// usb_get_cycle_delta
//-----------------------------------------------------------------
uint16_t usb_get_cycle_delta(uint32_t value)
{
    return ((value >> LOG_CTRL_CYCLE_L) & LOG_CTRL_CYCLE_MASK) << 8;
}
//-----------------------------------------------------------------
// usb_get_rst_state
//-----------------------------------------------------------------
int usb_get_rst_state(uint32_t value)
{
    return ((value >> LOG_RST_STATE_L) & LOG_RST_STATE_MASK) << 8;
}
//-----------------------------------------------------------------
// usb_get_token_device
//-----------------------------------------------------------------
uint8_t usb_get_token_device(uint32_t value)
{
    uint16_t data = ((value >> LOG_TOKEN_DATA_L) & LOG_TOKEN_DATA_MASK);
    return data & 0x7F;
}
//-----------------------------------------------------------------
// usb_get_token_endpoint
//-----------------------------------------------------------------
uint8_t usb_get_token_endpoint(uint32_t value)
{
    uint16_t data = ((value >> LOG_TOKEN_DATA_L) & LOG_TOKEN_DATA_MASK);
    return (data >> 7) & 0xF;
}
//-----------------------------------------------------------------
// usb_get_sof_crc5
//-----------------------------------------------------------------
uint8_t usb_get_sof_crc5(uint32_t value)
{
    return usb_crc5(usb_get_sof_frame(value));
}
//-----------------------------------------------------------------
// usb_get_token_crc5
//-----------------------------------------------------------------
uint8_t usb_get_token_crc5(uint32_t value)
{
    uint16_t data = ((value >> LOG_TOKEN_DATA_L) & LOG_TOKEN_DATA_MASK);
    return usb_crc5(data);
}
//-----------------------------------------------------------------
// usb_get_data_length
//-----------------------------------------------------------------
uint16_t usb_get_data_length(uint32_t value)
{
    return ((value >> LOG_DATA_LEN_L) & LOG_DATA_LEN_MASK);
}
//-----------------------------------------------------------------
// usb_get_sof_frame
//-----------------------------------------------------------------
uint16_t usb_get_sof_frame(uint32_t value)
{
    return ((value >> LOG_SOF_FRAME_L) & LOG_SOF_FRAME_MASK);
}
//-----------------------------------------------------------------
// usb_get_pid_str
//-----------------------------------------------------------------
char* usb_get_pid_str(uint8_t pid)
{
    static char unknown[16];
    switch (pid)
    {
        // Token
        case PID_OUT:
            return "OUT";
        case PID_IN:
            return "IN";
        case PID_SOF:
            return "SOF";
        case PID_SETUP:
            return "SETUP";
        case PID_PING:
            return "PING";
        // Data
        case PID_DATA0:
            return "DATA0";
        case PID_DATA1:
            return "DATA1";
        case PID_DATA2:
            return "DATA2";
        case PID_MDATA:
            return "MDATA";
        // Handshake
        case PID_ACK:
            return "ACK";
        case PID_NAK:
            return "NAK";
        case PID_STALL:
            return "STALL";
        case PID_NYET:
            return "NYET";
        // Special
        case PID_PRE:
            return "PRE/ERR";
        case PID_SPLIT:
            return "SPLIT";
        default: 
            break;        
    }
    
    sprintf(unknown, "UNKNOWN %02x", pid);
    return unknown;
}
