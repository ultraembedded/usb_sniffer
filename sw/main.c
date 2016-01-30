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
#include <assert.h>
#include <sys/time.h> 
#include <ftdi.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>

#include "log_format.h"
#include "usb_defs.h"
#include "usb_helpers.h"
#include "usb_sniffer.h"
#include "log_file.h"

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
#define LA_BUFFER_BASE      0x00000000
#define LA_BUFFER_SIZE      (64 * 1024)

//-----------------------------------------------------------------
// write_usb_file
//-----------------------------------------------------------------
static int write_usb_file(const char *output_file, FILE *tmpf, tUsbSpeed speed)
{
    int err = 0;
    int i,j;
    uint32_t value = 0;
    uint32_t data = 0;
    uint8_t  usb_data[MAX_PACKET_SIZE];
    int      usb_idx;
    int      idx = 0;
    long     buf_size = 0;

    // Get size of temporary file
    fseek(tmpf, 0, SEEK_END);
    buf_size = ftell(tmpf);
    rewind(tmpf);

    // No captured data...
    if (buf_size == 0)
        return 0;

    // Create log file
    if (log_file_create(output_file) != 0)
        return -1;

    // Iterate through log and convert to .usb format file
    while (idx < (buf_size / 4))
    {
        fread(&value, 4, 1, tmpf);
        idx++;

        switch ((value >> LOG_CTRL_TYPE_L) & LOG_CTRL_CYCLE_MASK)
        {
            case LOG_CTRL_TYPE_SOF:
            {                
                log_file_add_sof(value, speed == USB_SPEED_HS);
            }
            break;
            case LOG_CTRL_TYPE_RST:
            {
                log_file_add_rst(value, speed == USB_SPEED_HS);
            }
            break;
            case LOG_CTRL_TYPE_TOKEN:
            {
                log_file_add_token(value);
            }
            break;
            case LOG_CTRL_TYPE_HSHAKE:
            {
                log_file_add_handshake(value);
            }
            break;
            case LOG_CTRL_TYPE_DATA:
            {
                uint32_t len = usb_get_data_length(value);

                usb_idx = 0;
                for (i = 0; i < len; i+= 4)
                {
                    fread(&data, 4, 1, tmpf);
                    idx++;
                    
                    for (j=0;j<4 && usb_idx < len;j++)
                        usb_data[usb_idx++] = data >> (8 * j);
                }

                log_file_add_data(value, usb_data, len);
            }
            break;
            default:
                printf("ERROR: Unknown ID %x\n", value);
                return -1;
        }
    }

    log_file_close();

    return err;
}
//-----------------------------------------------------------------
// user_abort_check
//-----------------------------------------------------------------
static int user_abort_check(void)
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}
//-----------------------------------------------------------------
// main
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    char *filename = "capture.usb";
    int res;
    int c;
    int help = 0;

    int dev_addr = -1;
    int endpoint = -1;
    int disable_sof = 0;
    int cont_mode = 1;
    int decode_log_file = 0;
    int inverse_match = 0;
    tUsbSpeed speed = USB_SPEED_HS;
    
    while ((c = getopt (argc, argv, "d:e:slf:nu:")) != -1)
    {
        switch(c)
        {
            case 'd': // Device
                 dev_addr = (int)strtoul(optarg, NULL, 0);
                break;
            case 'e': // Endpoint
                 endpoint = (int)strtoul(optarg, NULL, 0);
                break;
            case 's': // Drop SOF
                 disable_sof = 1;
                break;
            case 'l': // One shot mode (stop on buffer full)
                 cont_mode = 0;
                break;
            case 'f': // Filename
                filename = optarg;
                break;
            case 'n':
                inverse_match = 1;
                break;
            case 'u': // Speed
                if (strcmp(optarg, "hs") == 0)
                    speed = USB_SPEED_HS;
                else if (strcmp(optarg, "fs") == 0)
                    speed = USB_SPEED_FS;
                else if (strcmp(optarg, "ls") == 0)
                    speed = USB_SPEED_LS;
                else
                {
                    fprintf (stderr,"ERROR: Incorrect speed selection\n");
                    help = 1;
                }
                break;
            default:
                help = 1;
                break;
        }
    }

    if (help)
    {
        fprintf (stderr,"Usage:\n");
        fprintf (stderr,"-d 0xnn     - Match only this device ID\n");
        fprintf (stderr,"-e 0xnn     - Match only this endpoint\n");
        fprintf (stderr,"-u ls|fs|hs - USB speed\n");
        fprintf (stderr,"-n          - Inverse matching (exclude device / endpoint)\n");
        fprintf (stderr,"-s          - Disable SOF collection (breaks timing info)\n");
        fprintf (stderr,"-l          - One shot mode (stop on single buffer full)\n");
        fprintf (stderr,"-f          - Capture file to either .txt, .raw, .usb (default: capture.usb)\n");
        exit(-1);
    }

    // Capture mode
    if (usb_sniffer_init() != 0)
    {
        fprintf(stderr, "Error: Cannot access HW, try SUDOing\n");
        return -1;
    }

    // Disable probe
    usb_sniffer_stop();

    // Configure device
    usb_sniffer_setup_mem(LA_BUFFER_BASE, LA_BUFFER_SIZE);
    usb_sniffer_match_device(dev_addr, inverse_match);
    usb_sniffer_match_endpoint(endpoint, inverse_match);
    usb_sniffer_drop_sof(disable_sof);
    usb_sniffer_continuous_mode(0);
    usb_sniffer_set_speed(speed);

    uint32_t rd_ptr = 0;
    usb_sniffer_set_rd_ptr(rd_ptr);

    FILE *fout = tmpfile();

    // Enable probe
    usb_sniffer_start();

    // One shot mode
    if (!cont_mode)
    {
        printf("Sampling: Press <ENTER> to abort\n");
        do
        {
            if (user_abort_check())
                break;

            usleep(10 * 1000);
        }
        while (!usb_sniffer_wrapped());

        uint32_t wr_ptr = usb_sniffer_current(NULL);

        uint32_t size;
        if (wr_ptr > rd_ptr)
            size = wr_ptr - rd_ptr + 4;
        else if (wr_ptr < rd_ptr)
            size = LA_BUFFER_SIZE - rd_ptr + wr_ptr + 4;
        else
            size = 0;

        printf("Captured %d bytes of data\n", size);

        if (size != 0)
            usb_sniffer_extract_buffer(fout, rd_ptr, size);
    }
    // Continuous capture mode
    else
    {
        printf("Sampling: Press <ENTER> to stop\n");
        uint32_t data_count = 0;
        int overflow = 0;
        uint32_t last_wr = 0;
        do
        {
            if (user_abort_check())
                break;

            // Get current write pointer
            uint32_t wr_ptr = usb_sniffer_current(&overflow);

            // Calculate delta between rd & wr pointers
            uint32_t size;
            if (wr_ptr > rd_ptr)
                size = wr_ptr - rd_ptr + 4;
            else if (wr_ptr < rd_ptr)
                size = LA_BUFFER_SIZE - rd_ptr + wr_ptr + 4;
            else
                size = 0;

            // Copy data between RD & WR pointers to capture file
            if (size != 0 && wr_ptr != last_wr)
            {
                if (usb_sniffer_extract_buffer(fout, rd_ptr, size) == -1)
                    break;

                // Update read pointer
                rd_ptr = wr_ptr + 4;
                usb_sniffer_set_rd_ptr(rd_ptr);
                
                last_wr = wr_ptr;

                if ((data_count / 1024) != ((data_count + size) / 1024))
                    printf("\r%dKB ", ((data_count + size) / 1024));

                data_count += size;
            }

            // Buffer overflow - data not trusted
            if (overflow)
            {
                printf("\nBuffer overrun - abort!\n");
                break;
            }
        }
        while (1);
    }

    // Write output file
    write_usb_file(filename, fout, speed);

    // Close temp file
    fclose(fout);

    // Disable probe
    usb_sniffer_stop();

    usb_sniffer_close();

    return 0;
}

