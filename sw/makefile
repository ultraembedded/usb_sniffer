###############################################################################
## Makefile
###############################################################################

# Target
TARGET     ?= usb_sniffer

# Options
CFLAGS      = 
LDFLAGS     = 
LIBS        = -lftdi

ARGS       += 

# Source Files
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC)) 

###############################################################################
# Rules
###############################################################################
all: $(TARGET)
    
clean:
	-rm $(OBJ) $(TARGET)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

%.o : %.cpp
	g++ -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJ)
	g++ $(LDFLAGS) $(OBJ) $(LIBS) -o $@

run: $(TARGET)
	sudo ./$(TARGET) $(ARGS)
