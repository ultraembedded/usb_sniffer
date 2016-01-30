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
#include "usb_sniffer.h"
#include "usb_sniffer_regs.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define CFG_BASE_ADDR        0x80000000
#define CHUNK_SIZE           2048

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static uint32_t _mem_base = 0;
static uint32_t _mem_size = 0;
static uint32_t _cfg_reg  = 0;

//-----------------------------------------------------------------
// usb_sniffer_init
//-----------------------------------------------------------------
int usb_sniffer_init(void)
{
    if (ftdi_hw_init() != 0)
        return -1;

    return 0;
}
//-----------------------------------------------------------------
// usb_sniffer_close
//-----------------------------------------------------------------
int usb_sniffer_close(void)
{
    ftdi_hw_close();
    return 0;
}
//-----------------------------------------------------------------
// set_base
//-----------------------------------------------------------------
static int set_base(uint32_t addr)
{
    uint32_t readback = 0;

    ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_BASE, addr);
    ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_BASE, &readback);

    if (readback != addr)
        fprintf(stderr, "ERROR: Failed to write buffer address\n");

    return readback == addr;
}
//-----------------------------------------------------------------
// set_end
//-----------------------------------------------------------------
static int set_end(uint32_t addr)
{
    uint32_t readback = 0;

    ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_END, addr);
    ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_END, &readback);

    if (readback != addr)
        fprintf(stderr, "ERROR: Failed to write buffer address\n");

    return readback == addr;
}
//-----------------------------------------------------------------
// usb_sniffer_set_rd_ptr
//-----------------------------------------------------------------
int usb_sniffer_set_rd_ptr(uint32_t addr)
{
    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_READ, addr);
}
//-----------------------------------------------------------------
// usb_sniffer_setup_mem
//-----------------------------------------------------------------
int usb_sniffer_setup_mem(uint32_t base, uint32_t size)
{
    uint32_t end = base + size - 4;

    if (!set_base(base))
        return 0;

    if (!set_end(end))
        return 0;

    _mem_base = base;
    _mem_size = size;

    return 0;
}
//-----------------------------------------------------------------
// usb_sniffer_match_device
//-----------------------------------------------------------------
int usb_sniffer_match_device(int dev, int exclude)
{
    _cfg_reg &= ~(USB_BUFFER_CFG_DEV_MASK << USB_BUFFER_CFG_DEV_SHIFT);
    _cfg_reg &= ~(USB_BUFFER_CFG_MATCH_DEV_MASK << USB_BUFFER_CFG_MATCH_DEV_SHIFT);
    _cfg_reg &= ~(USB_BUFFER_CFG_EXCLUDE_DEV_MASK << USB_BUFFER_CFG_EXCLUDE_DEV_SHIFT);

    if (dev >= 0)
    {
        _cfg_reg |= (dev << USB_BUFFER_CFG_DEV_SHIFT);

        if (exclude)
            _cfg_reg |= (1   << USB_BUFFER_CFG_EXCLUDE_DEV_SHIFT);
        else
            _cfg_reg |= (1   << USB_BUFFER_CFG_MATCH_DEV_SHIFT);
    }

    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_match_endpoint
//-----------------------------------------------------------------
int usb_sniffer_match_endpoint(int ep, int exclude)
{
    _cfg_reg &= ~(USB_BUFFER_CFG_EP_MASK << USB_BUFFER_CFG_EP_SHIFT);
    _cfg_reg &= ~(USB_BUFFER_CFG_MATCH_EP_MASK << USB_BUFFER_CFG_MATCH_EP_SHIFT);
    _cfg_reg &= ~(USB_BUFFER_CFG_EXCLUDE_EP_MASK << USB_BUFFER_CFG_EXCLUDE_EP_SHIFT);

    if (ep >= 0)
    {
        _cfg_reg |= (ep << USB_BUFFER_CFG_EP_SHIFT);

        if (exclude)
            _cfg_reg |= (1  << USB_BUFFER_CFG_EXCLUDE_EP_SHIFT);
        else
            _cfg_reg |= (1  << USB_BUFFER_CFG_MATCH_EP_SHIFT);
    }

    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_drop_sof
//-----------------------------------------------------------------
int usb_sniffer_drop_sof(int enable)
{
    if (enable)
        _cfg_reg |= (USB_BUFFER_CFG_IGNORE_SOF_MASK << USB_BUFFER_CFG_IGNORE_SOF_SHIFT);
    else
        _cfg_reg &= ~(USB_BUFFER_CFG_IGNORE_SOF_MASK << USB_BUFFER_CFG_IGNORE_SOF_SHIFT);

    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_continuous_mode
//-----------------------------------------------------------------
int usb_sniffer_continuous_mode(int enable)
{
    if (enable)
        _cfg_reg |= (USB_BUFFER_CFG_CONT_MASK << USB_BUFFER_CFG_CONT_SHIFT);
    else
        _cfg_reg &= ~(USB_BUFFER_CFG_CONT_MASK << USB_BUFFER_CFG_CONT_SHIFT);

    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_set_speed
//-----------------------------------------------------------------
int usb_sniffer_set_speed(tUsbSpeed speed)
{
    _cfg_reg &= ~(USB_BUFFER_CFG_SPEED_MASK << USB_BUFFER_CFG_SPEED_SHIFT);
    _cfg_reg |= (speed << USB_BUFFER_CFG_SPEED_SHIFT);

    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_start
//-----------------------------------------------------------------
int usb_sniffer_start(void)
{
    _cfg_reg |= (USB_BUFFER_CFG_ENABLED_MASK << USB_BUFFER_CFG_ENABLED_SHIFT);
    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_stop
//-----------------------------------------------------------------
int usb_sniffer_stop(void)
{
    _cfg_reg &= ~(USB_BUFFER_CFG_ENABLED_MASK << USB_BUFFER_CFG_ENABLED_SHIFT);
    return ftdi_hw_mem_write_word(CFG_BASE_ADDR + USB_BUFFER_CFG, _cfg_reg);
}
//-----------------------------------------------------------------
// usb_sniffer_triggered
//-----------------------------------------------------------------
int usb_sniffer_triggered(void)
{
    uint32_t status = 0;

    if (ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_STS, &status) != sizeof(uint32_t))
    {
        fprintf(stderr, "ERROR: Failed to read status\n");
        return 0;
    }

    if (status & (1 << USB_BUFFER_STS_TRIG_SHIFT))
        return 1;
    else
        return 0;
}
//-----------------------------------------------------------------
// usb_sniffer_wrapped
//-----------------------------------------------------------------
int usb_sniffer_wrapped(void)
{
    uint32_t status = 0;

    if (ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_STS, &status) != sizeof(uint32_t))
    {
        fprintf(stderr, "ERROR: Failed to read status\n");
        return 0;
    }

    if (status & (1 << USB_BUFFER_STS_WRAPPED_SHIFT))
        return 1;
    else
        return 0;
}
//-----------------------------------------------------------------
// usb_sniffer_overrun
//-----------------------------------------------------------------
int usb_sniffer_overrun(void)
{
    uint32_t status = 0; 

    if (ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_STS, &status) != sizeof(uint32_t))
    {
        fprintf(stderr, "ERROR: Failed to read status\n");
        return 0;
    }

    if (status & (1 << USB_BUFFER_STS_OVERFLOW_SHIFT))
        return 1;
    else
        return 0;
}
//-----------------------------------------------------------------
// usb_sniffer_current
//-----------------------------------------------------------------
uint32_t usb_sniffer_current(int *overflow)
{
    uint32_t current = 0;

    // Want overflow status? Read block of 2 words (more effecient)
    if (overflow)
    {
        uint32_t read_buf[2];
        if (ftdi_hw_mem_read(CFG_BASE_ADDR + USB_BUFFER_STS, read_buf, (2 * sizeof(uint32_t))) != (2 * sizeof(uint32_t)))
        {
            fprintf(stderr, "ERROR: Failed to read status\n");
            return 0;
        }
    
        if (read_buf[0] & (1 << USB_BUFFER_STS_OVERFLOW_SHIFT))
            *overflow = 1;
        else
            *overflow = 0;

        current = read_buf[1];
    }
    else if (ftdi_hw_mem_read_word(CFG_BASE_ADDR + USB_BUFFER_CURRENT, &current) != sizeof(uint32_t))
    {
        fprintf(stderr, "ERROR: Failed to read status\n");
        return 0;
    }

    return current;
}
//-----------------------------------------------------------------
// usb_sniffer_base/end
//-----------------------------------------------------------------
uint32_t usb_sniffer_base(void) { return _mem_base; }
uint32_t usb_sniffer_end(void)  { return _mem_base + _mem_size - 4; }
//-----------------------------------------------------------------
// usb_sniffer_read_buffer
//-----------------------------------------------------------------
int usb_sniffer_read_buffer(uint8_t *buffer, uint32_t base, int size)
{
    int err = 0;
    int i;
    int chunk;
    int remain;

    for (i=0;i<size;)
    {
        chunk = (size - i);
        if (chunk > CHUNK_SIZE)
            chunk = CHUNK_SIZE;

        remain = _mem_size - base - _mem_base;
        if (chunk > remain)
            chunk = remain;

        if (ftdi_hw_mem_read(base, &buffer[i], chunk) != chunk)
        {
            fprintf(stderr, "Download: Error downloading file\n");
            err = 1;
            break;
        }        

        i += chunk;
        base += chunk;

        if (base >= (_mem_base + _mem_size))
            base = _mem_base;
    }

    return err ? -1: 0;
}
//-----------------------------------------------------------------
// usb_sniffer_extract_buffer: Extract buffer from target and write to file
//-----------------------------------------------------------------
int usb_sniffer_extract_buffer(FILE *f, uint32_t rd_ptr, uint32_t size)
{
    int err = 0;
    int i,j;
    uint32_t value = 0;
    uint32_t data = 0;

    uint32_t *buffer = (uint32_t *)malloc(size);
    assert(buffer);

    assert(!(size & 3));

    // Extract buffer from target
    if (usb_sniffer_read_buffer((uint8_t*)buffer, rd_ptr, size) != 0)
    {
        free(buffer);
        buffer = NULL;
        return -1;
    }

    // Fix data packet ordering
    int rd_idx = (size / 4) - 1;
    int count = size / 4;

    while (count > 0)
    {
        value = buffer[rd_idx--];
        count--;

        switch ((value >> LOG_CTRL_TYPE_L) & LOG_CTRL_CYCLE_MASK)
        {
            case LOG_CTRL_TYPE_SOF:
            case LOG_CTRL_TYPE_RST:
            case LOG_CTRL_TYPE_TOKEN:
            case LOG_CTRL_TYPE_HSHAKE:
                break;
            case LOG_CTRL_TYPE_DATA:
            {
                uint32_t len = usb_get_data_length(value);

                //printf("DATA: Len %d\n", len);

                // Shuffle to put control word as first word instead of last
                for (i = 0; i < len; i+= 4)
                {
                    if (rd_idx < 0)
                    {
                        fprintf(stderr, "ERROR: Bad data index\n");
                        err = 1;
                        count = 0;
                        break;
                    }

                    buffer[rd_idx + 1] = buffer[rd_idx];
                    rd_idx--;
                    count--;
                }

                buffer[rd_idx + 1] = value;
            }
            break;
            default:
                fprintf(stderr, "ERROR: Unknown ID %x\n", value);
                err = 1;
                count = 0;
                break;
        }
    }

    // Write fixed buffer to file
    if (f != NULL && !err)
        fwrite(buffer, 1, size, f);

    free(buffer);
    buffer = NULL;
    return err ? -1 : size;
}
