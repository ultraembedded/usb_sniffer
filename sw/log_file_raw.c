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
#include "log_file_raw.h"

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static FILE *_file;

//-----------------------------------------------------------------
// raw_file_add_sof: Add start of frame token to log
//-----------------------------------------------------------------
int raw_file_add_sof(uint32_t value, int is_hs)
{
    int i;
    uint16_t frame_num = usb_get_sof_frame(value);
    uint8_t  crc5      = usb_get_sof_crc5(value);
    uint16_t len       = 3;

    uint8_t sof_data[3];

    sof_data[0] = PID_SOF;
    sof_data[1] = frame_num & 0xFF;
    sof_data[2] = (frame_num >> 8) & 0x7;
    sof_data[2]|= (crc5 << 3);

    fwrite(&len, 1, 2, _file);
    fwrite(sof_data, 1, sizeof(sof_data), _file);

    return 0;
}
//-----------------------------------------------------------------
// raw_file_add_rst: Add reset event to the log
//-----------------------------------------------------------------
int raw_file_add_rst(uint32_t value, int is_hs)
{
    return 0;
}
//-----------------------------------------------------------------
// raw_file_add_token: Add token (IN, OUT, SETUP, PING)
//-----------------------------------------------------------------
int raw_file_add_token(uint32_t value)
{
    uint8_t token[3];

    uint8_t pid          = usb_get_pid(value);
    uint8_t device       = usb_get_token_device(value);
    uint8_t endpoint     = usb_get_token_endpoint(value);
    uint8_t crc5         = usb_get_token_crc5(value);
    uint16_t len         = 3;

    token[0] = pid;
    token[1] = device & 0x7F;
    token[1]|= (endpoint << 7) & 0x80;
    token[2] = (endpoint >> 1) & 0x7;
    token[2]|= (crc5 << 3);

    fwrite(&len, 1, 2, _file);
    fwrite(token, 1, sizeof(token), _file);

    return 0;
}
//-----------------------------------------------------------------
// raw_file_add_handshake: Add handshake (ACK, NAK, NYET)
//-----------------------------------------------------------------
int raw_file_add_handshake(uint32_t value)
{
    uint8_t  pid = usb_get_pid(value);
    uint16_t len = 1;
    fwrite(&len, 1, 2, _file);
    fwrite(&pid, 1, 1, _file);
    return 0;
}
//-----------------------------------------------------------------
// raw_file_add_data: Add data packet to log
//-----------------------------------------------------------------
int raw_file_add_data(uint32_t value, uint8_t *data, int length)
{
    uint8_t  pid = usb_get_pid(value);
    uint16_t len = 1 + length;

    fwrite(&len, 1, 2, _file);
    fwrite(&pid, 1, 1, _file);
    fwrite(data, 1, length, _file);

    return 0;
}
//-----------------------------------------------------------------
// raw_file_create: Create & open empty log file
//-----------------------------------------------------------------
int raw_file_create(const char *filename)
{
    _file = fopen(filename, "wb");   
    return _file == NULL ? -1 : 0;
}
//-----------------------------------------------------------------
// raw_file_close: Close open file handle
//-----------------------------------------------------------------
int raw_file_close(void)
{
    if (_file != NULL)
        fclose(_file);

    _file = NULL;

    return 0;
}
