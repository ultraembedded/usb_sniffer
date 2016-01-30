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
#include "log_file_txt.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define TICKS_PER_HS_UFRAME        7500
#define TICKS_PER_FSLS_FRAME       60000

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static FILE *_file;
static uint32_t _last_tic = 0;
static int _in_rst = -1;

//-----------------------------------------------------------------
// txt_file_add_sof: Add start of frame token to log
//-----------------------------------------------------------------
int txt_file_add_sof(uint32_t value, int is_hs)
{
    int i;
    uint16_t frame_num = usb_get_sof_frame(value);

    // Work out delta between last message and next SOF boundary
    int tics_per_frame = is_hs ? TICKS_PER_HS_UFRAME : TICKS_PER_FSLS_FRAME;
    int delta = tics_per_frame - _last_tic;
    if (delta <= 0)
        delta = 1;

    fprintf(_file, "SOF - Frame %d\n", frame_num);

    _last_tic = 0;

    return 0;    
}
//-----------------------------------------------------------------
// txt_file_add_rst: Add reset event to the log
//-----------------------------------------------------------------
int txt_file_add_rst(uint32_t value, int is_hs)
{
    int in_rst = usb_get_rst_state(value);

    if (in_rst != _in_rst)
    {
        fprintf(_file, "USB RST = %d\n", in_rst);

        _last_tic = 0;

        _in_rst = in_rst;
    }

    return 0;
}
//-----------------------------------------------------------------
// txt_file_add_token: Add token (IN, OUT, SETUP, PING)
//-----------------------------------------------------------------
int txt_file_add_token(uint32_t value)
{
    uint8_t pid          = usb_get_pid(value);
    uint8_t device       = usb_get_token_device(value);
    uint8_t endpoint     = usb_get_token_endpoint(value);

    fprintf(_file, "%s Device %d Endpoint %d\n", usb_get_pid_str(pid), device, endpoint);

    return 0;
}
//-----------------------------------------------------------------
// txt_file_add_handshake: Add handshake (ACK, NAK, NYET)
//-----------------------------------------------------------------
int txt_file_add_handshake(uint32_t value)
{
    uint8_t pid = usb_get_pid(value);
    fprintf(_file, "  %s\n", usb_get_pid_str(pid));
    return 0;
}
//-----------------------------------------------------------------
// txt_file_add_data: Add data packet to log
//-----------------------------------------------------------------
int txt_file_add_data(uint32_t value, uint8_t *data, int length)
{
    uint8_t pid = usb_get_pid(value);
    int i;

    fprintf(_file, "  %s: Length %d\n", usb_get_pid_str(pid), length-2);

    fprintf(_file, "  ");
    for (i=0;i<length-2;i++)
    {
        fprintf(_file, "%02x ", data[i]);

        if (!((i+1) & 0xF) || ((i+1) == (length-2)))
            fprintf(_file, "\n  ");
    }
    fprintf(_file, "CRC = %02x%02x\n", data[length-2], data[length-1]);

    return 0;
}
//-----------------------------------------------------------------
// txt_file_create: Create & open empty log file
//-----------------------------------------------------------------
int txt_file_create(const char *filename)
{
    _file = fopen(filename, "w");
    return _file == NULL ? -1 : 0;
}
//-----------------------------------------------------------------
// txt_file_close: Close open file handle
//-----------------------------------------------------------------
int txt_file_close(void)
{
    if (_file != NULL)
        fclose(_file);

    _file = NULL;

    return 0;
}
