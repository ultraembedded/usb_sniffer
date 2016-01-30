## USB Sniffer
High Speed USB 2.0 capture device based on miniSpartan6+

Homepage: [http://ultra-embedded.com/usb_sniffer](http://ultra-embedded.com/usb_sniffer)  
Github:   [http://github.com/ultraembedded/usb_sniffer](http://github.com/ultraembedded/usb_sniffer)

This project is a USB 2.0 high speed (480mbps) capable bus capture device built from a low cost FPGA development board and USB 2.0 PHY evaluation board.

The FPGA development board is the miniSpartan6+, based around Xilinx Spartan 6 LX9 (there is also a LX25 version available), which has 32MB SDRAM, FTDI USB 2.0 interface, SPI Flash, SD card, 2 HDMI interfaces and numerous digital and analog I/O connections for expansion.

The USB 2.0 PHY interface is based on the SMSC/Microchip USB3300 which is a ULPI based high speed USB PHY. 
The 'USB3300 USB HS Board' evaluation board has both USB A and USB B mini connectors which are wired together, making it suited to constructing a cheap USB bus analyser from!

The miniSpartan6+ is available from [https://www.scarabhardware.com/minispartan6][1] for $75 and USB3300 PHY board is available from [http://www.waveshare.com/usb3300-usb-hs-board.htm][2] for $9.

#### Features

* Support HS (480mbps), FS (12mbps) and LS (1.5mbps) captures.
* Supports continuous streaming, one shot and detects buffer overruns.
* Dense logging format that is expanded on the host PC.
* Filtering based on match or not match of device ID and/or endpoint
* Filtering of SOF packets (which are every 125uS in HS USB, 1ms in FS USB).
* 0.5uS timing resolution (HS) or 4uS timing resolution (FS/LS)

[1]: https://www.scarabhardware.com/minispartan6
[2]: http://www.waveshare.com/usb3300-usb-hs-board.htm