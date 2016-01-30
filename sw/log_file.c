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

#include "log_file.h"
#include "log_file_usb.h"
#include "log_file_raw.h"
#include "log_file_txt.h"

//-----------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------
struct log_func
{
    int (*create)(const char *filename);
    int (*close)(void);
    int (*add_sof)(uint32_t value, int is_hs);
    int (*add_rst)(uint32_t value, int is_hs);
    int (*add_token)(uint32_t value);
    int (*add_handshake)(uint32_t value);
    int (*add_data)(uint32_t value, uint8_t *data, int length);
};

enum eLogFormats { LOG_FMT_USB, LOG_FMT_RAW, LOG_FMT_TXT, LOG_FMT_MAX };

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static struct log_func _log_fmts[LOG_FMT_MAX] = 
{
    [LOG_FMT_USB] = 
    {
        .create         = usb_file_create,
        .close          = usb_file_close,
        .add_sof        = usb_file_add_sof,
        .add_rst        = usb_file_add_rst,
        .add_token      = usb_file_add_token,
        .add_handshake  = usb_file_add_handshake,
        .add_data       = usb_file_add_data
    },
    [LOG_FMT_RAW] = 
    {
        .create         = raw_file_create,
        .close          = raw_file_close,
        .add_sof        = raw_file_add_sof,
        .add_rst        = raw_file_add_rst,
        .add_token      = raw_file_add_token,
        .add_handshake  = raw_file_add_handshake,
        .add_data       = raw_file_add_data
    },
    [LOG_FMT_TXT] = 
    {
        .create         = txt_file_create,
        .close          = txt_file_close,
        .add_sof        = txt_file_add_sof,
        .add_rst        = txt_file_add_rst,
        .add_token      = txt_file_add_token,
        .add_handshake  = txt_file_add_handshake,
        .add_data       = txt_file_add_data
    }
};

static struct log_func *_log = NULL;

//-----------------------------------------------------------------
// log_file_create: Create & open empty log file
//-----------------------------------------------------------------
int log_file_create(const char *filename)
{
    char *ext = strrchr(filename, '.');
    if (ext && strcmp(ext, ".usb") == 0)
        _log = &_log_fmts[LOG_FMT_USB];
    else if (ext && strcmp(ext, ".raw") == 0)
        _log = &_log_fmts[LOG_FMT_RAW];
    else if (ext && strcmp(ext, ".txt") == 0)
        _log = &_log_fmts[LOG_FMT_TXT];
    else
    {
        fprintf (stderr,"ERROR: Unsupported output format (check extension)\n");
        return -1;
    }

    return _log->create(filename);
}
//-----------------------------------------------------------------
// log_file_close: Close open file handle
//-----------------------------------------------------------------
int log_file_close(void)
{
    return _log->close();
    _log = NULL;
}
//-----------------------------------------------------------------
// log_file_add_sof: Add start of frame token to log
//-----------------------------------------------------------------
int log_file_add_sof(uint32_t value, int is_hs)
{
    return _log->add_sof(value, is_hs);
}
//-----------------------------------------------------------------
// log_file_add_rst: Add reset event to the log
//-----------------------------------------------------------------
int log_file_add_rst(uint32_t value, int is_hs)
{
    return _log->add_rst(value, is_hs);
}
//-----------------------------------------------------------------
// log_file_add_token: Add token (IN, OUT, SETUP, PING)
//-----------------------------------------------------------------
int log_file_add_token(uint32_t value)
{
    return _log->add_token(value);
}
//-----------------------------------------------------------------
// log_file_add_handshake: Add handshake (ACK, NAK, NYET)
//-----------------------------------------------------------------
int log_file_add_handshake(uint32_t value)
{
    return _log->add_handshake(value);
}
//-----------------------------------------------------------------
// log_file_add_data: Add data packet to log
//-----------------------------------------------------------------
int log_file_add_data(uint32_t value, uint8_t *data, int length)
{
    return _log->add_data(value, data, length);
}
