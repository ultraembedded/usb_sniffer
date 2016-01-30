PROJECT=fpga
PART=xc6slx9-ftg256-3
CABLE ?= ftdi

TOOL_PATH ?= /opt/Xilinx/14.5/ISE_DS/ISE/bin/lin64
CORES_PATH = ../cores
RTL_ROOT   = ${CORES_PATH}/usb_sniffer/rtl
FTDI_PATH  = ${CORES_PATH}/ftdi_async_bridge/rtl
ULPI_PATH  = ${CORES_PATH}/ulpi_wrapper/rtl

all: ${PROJECT}_routed.bit

project/${PROJECT}.prj: 
	mkdir -p project
	echo "verilog work \"../top.v\"" > project/${PROJECT}.prj
	echo "verilog work \"../top_core.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../${FTDI_PATH}/ftdi_if.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../${FTDI_PATH}/ftdi_sync.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../${ULPI_PATH}/ulpi_wrapper.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../${RTL_ROOT}/usb_sniffer.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../${RTL_ROOT}/usb_sniffer_regs.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../ram_wb.v\"" >> project/${PROJECT}.prj
	echo "verilog work \"../clkgen_pll.v\"" >> project/${PROJECT}.prj

${PROJECT}.ngc: project/${PROJECT}.prj	
	cp ${PROJECT}.* project/
	mkdir -p project/xst/projnav.tmp/
	cd project; ${TOOL_PATH}/xst -intstyle ise -ifn ${PROJECT}.xst -ofn ${PROJECT}.syr
        
${PROJECT}.ngd: ${PROJECT}.ngc
	cd project; ${TOOL_PATH}/ngdbuild -intstyle ise -dd _ngo -nt timestamp \
	-uc ${PROJECT}.ucf -p ${PART} ${PROJECT}.ngc ${PROJECT}.ngd

${PROJECT}.ncd: ${PROJECT}.ngd
	cd project; ${TOOL_PATH}/map -w -intstyle ise -p ${PART} \
	-detail -ir off -ignore_keep_hierarchy -pr b -timing -ol high -logic_opt on  \
	-o ${PROJECT}.ncd ${PROJECT}.ngd ${PROJECT}.pcf 

${PROJECT}_routed.ncd: ${PROJECT}.ncd
	cd project; ${TOOL_PATH}/par -w -intstyle ise -ol high ${PROJECT}.ncd ${PROJECT}_routed.ncd ${PROJECT}.pcf

${PROJECT}_routed.bit: ${PROJECT}_routed.ncd
	cd project; ${TOOL_PATH}/bitgen -f ${PROJECT}.ut ${PROJECT}_routed.ncd

load:
	sudo xc3prog/xc3sprog -c ${CABLE} project/${PROJECT}_routed.bit

clean:
	@rm -rf project
