CROSS_COMPILE ?= avr-

CC = ${CROSS_COMPILE}gcc
AS = ${CROSS_COMPILE}as
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump


# Files and directories
BUILD_DIR ?= build

TARGET_ELF = $(BUILD_DIR)/nanofs.elf
TARGET_LST = $(BUILD_DIR)/nanofs.lst
TARGET_SYM = $(BUILD_DIR)/nanofs.sym
TARGET_BIN = $(BUILD_DIR)/nanofs.bin
TARGET_HEX = $(BUILD_DIR)/nanofs.hex

SOURCES_C = $(shell find -name "*.c" | cut -d '/' -f 2-)
OBJECTS_C = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCES_C))

SOURCES_ASM = $(shell find -name "*.asm" | cut -d '/' -f 2-)
OBJECTS_ASM = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(SOURCES_ASM))

SUB_DIRS += $(shell dirname $(OBJECTS_C))
SUB_DIRS += $(shell dirname $(OBJECTS_ASM))

INCLUDE += include

# Filesystem image
NANOFS_IMAGE = nanofs.img
TARGET_NANOFS_IMAGE = $(BUILD_DIR)/$(NANOFS_IMAGE).o 


# Compiler and linker options
MARCH_FLAGS += -mmcu=atmega328p

ASFLAGS += -I$(INCLUDE) $(MARCH_FLAGS)

CCFLAGS += -I$(INCLUDE) $(MARCH_FLAGS)
CCFLAGS += -Wall -g -Os -mrelax
CCFLAGS += -nostdlib -ffreestanding
CCFLAGS += -Wl,--gc-sections

LDSCRIPT = avr328p.ld
LDFLAGS = -T $(LDSCRIPT)


# verbosity
ifeq ($(VERBOSE),y)
V :=
V_AS :=
V_CC :=
V_LD :=
V_OBJCOPY :=
V_OBJDUMP :=
else    # VERBOSE != y
V = @
V_AS = @echo "  AS      " $(patsubst $(CURDIR)/%,%,$@);
V_CC = @echo "  CC      " $(patsubst $(CURDIR)/%,%,$@);
V_LD = @echo "  LD      " $(patsubst $(CURDIR)/%,%,$@);
V_OBJCOPY = @echo "  OBJCOPY " $(patsubst $(CURDIR)/%,%,$@);
V_OBJDUMP = @echo "  OBJDUMP " $(patsubst $(CURDIR)/%,%,$@);
endif   # VERBOSE


# Build rules
all: $(BUILD_DIR) $(TARGET_ELF) $(TARGET_BIN) $(TARGET_LST) $(TARGET_SYM) $(TARGET_HEX)

run: all flash eeprom serial

$(BUILD_DIR):
	$(V)mkdir -p $(BUILD_DIR)
$(BUILD_DIR)/%:
	$(V)mkdir -p $@


$(BUILD_DIR)/%.img.o: %.img
	$(V_LD)${LD} -r -b binary -o $@ $<
	$(V_OBJCOPY)${OBJCOPY} --rename-section .data=.nanofs $@ $@
$(BUILD_DIR)/%.o: %.asm
	$(V_AS)${AS} $(ASFLAGS) -c $< -o $@
$(BUILD_DIR)/%.o: %.c
	$(V_CC)${CC} ${CCFLAGS} -c $< -o $@


$(TARGET_ELF): $(SUB_DIRS) $(OBJECTS_C) $(OBJECTS_ASM) $(TARGET_NANOFS_IMAGE)
	$(V_LD)${LD} ${LDFLAGS} $(OBJECTS_ASM) $(OBJECTS_C) $(TARGET_NANOFS_IMAGE) -o $@

$(TARGET_LST): $(TARGET_ELF)
	$(V_OBJDUMP)$(OBJDUMP) -d $< > $@

$(TARGET_SYM): $(TARGET_ELF)
	$(V_OBJDUMP)$(OBJDUMP) -t $< > $@

$(TARGET_BIN): $(TARGET_ELF)
	$(V_OBJCOPY)$(OBJCOPY) -O binary $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(V_OBJCOPY)$(OBJCOPY) -R .eeprom -O ihex $< $@


# Programmer
DUDE = avrdude
DUDECONF ?= avrdude.conf
COM ?= /dev/ttyUSB0

DUDEOPTS += -v -V -patmega328p
DUDEOPTS += -carduino -b57600 -D -P${COM}

flash: $(TARGET_HEX)
	${DUDE} ${DUDEOPTS} -Uflash:w:$(TARGET_HEX)

eeprom:
	${DUDE} ${DUDEOPTS} -Ueeprom:w:$(NANOFS_IMAGE)

# Console
BAUDRATE = 38400
MINICOM = minicom
MINICOMOPTS += -D $(COM) -b $(BAUDRATE)

serial:
	${MINICOM} ${MINICOMOPTS}


.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)
