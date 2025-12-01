include $(NANOFS_DIR)/Common.mk

export VERBOSE

CROSS_COMPILE := 

CC = ${CROSS_COMPILE}gcc
AS = ${CROSS_COMPILE}as
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

# Files and directories
BUILD_DIR = build

INCLUDE = $(NANOFS_DIR)/include/nanofs


CCFLAGS += -DCC_HOST -I$(INCLUDE)

# Tools definitions
MKFS = __mkfs

TARGET_TOOLS = \
	$(BUILD_DIR)/$(MKFS)


# Build rules
all: $(BUILD_DIR) $(TARGET_TOOLS)

$(BUILD_DIR):
	$(V)mkdir -p $(BUILD_DIR)
$(BUILD_DIR)/%:
	$(V)mkdir -p $@

__%: %.c
	$(V_CC)${CC} ${CCFLAGS} $< -o $@


# Tools usage
mkfs: $(MKFS)
	./$(MKFS)

clean:
	rm -rf $(BUILD_DIR)
