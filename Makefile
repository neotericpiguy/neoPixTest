PROJECT_DIR=$(shell git rev-parse --show-toplevel)
PROJECT=$(shell basename `pwd`)
TARGET=$(PROJECT).hex

#include $(PROJECT_DIR)/config.mk

DEVICE     = ATmega328p # See avr-help for all possible devices
CLOCK      = 16000000            # 8Mhz
PROGRAMMER =	-c arduino -P /dev/ttyACM0 -p m328p

COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)

all: $(TARGET)
	@echo -e "\e[32mBuilt $(TARGET)\e[0m"

#.c.o:
#	$(COMPILE) -c $< -o $@

$(PROJECT).hex: $(PROJECT).ino
	@mkdir -p $(PROJECT_DIR)/build 
	arduino-builder -build-path $(PROJECT_DIR)/build/ -fqbn archlinux-arduino:avr:uno -hardware /usr/share/arduino/hardware -tools /usr/bin $<
	@cp $(PROJECT_DIR)/build/$<.hex $(PROJECT_DIR)/$@

install: $(PROJECT).hex 
	sudo avrdude -FV $(PROGRAMMER) -U flash:w:$<:i 

clean:
	-rm -rf $(PROJECT_DIR)/build

distclean: clean
	-rm *.hex
