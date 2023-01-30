#FOR WINDOWS
MCU=atmega4809
F_CPU=20000000UL/6UL
PORT=COM8
PROGRAMMER=jtag2updi

CC=./../toolchain/bin/avr-gcc.exe
OBJCOPY=./../toolchain/bin/avr-objcopy.exe
AVRSIZE=./../toolchain/bin/avr-size.exe
CFLAGS=-Wall -Os -funsigned-char -funsigned-bitfields -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mmcu=${MCU} -DF_CPU=${F_CPU} -I.

MCU_DUDE=$(patsubst atmega%,m%, $(patsubst attiny%,t%,$(MCU)))
AVRDUDE=./../avrdude.exe
AVRDUDE_FLAGS=-B 64

TARGET=main
SRC_DIRS=.
BUILD_DIR=build

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)\${TARGET}: $(OBJS)
	${CC} ${CFLAGS} ${OBJS} -o $@.bin


	${OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures $@.bin $@.hex
	${AVRSIZE} --mcu=${MCU} -C $@.bin

$(BUILD_DIR)/%.c.o: %.c
	if not exist "$@.dir" mkdir "$@.dir"
	${CC} ${CFLAGS} -c $< -o $@

flash:
	${AVRDUDE} -p ${MCU_DUDE} ${AVRDUDE_FLAGS} -c ${PROGRAMMER} -U flash:w:${BUILD_DIR}\${TARGET}.hex:i -P ${PORT}

reset:
	${AVRDUDE} -p ${MCU_DUDE} ${AVRDUDE_FLAGS} -c ${PROGRAMMER} -P ${PORT}

.PHONY: clean

clean:
	rmdir /s ${BUILD_DIR}
