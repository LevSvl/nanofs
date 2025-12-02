include $(NANOFS_DIR)/Common.mk

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
MKFS = $(BUILD_DIR)/__mkfs

TARGET_TOOLS = \
	$(MKFS)


# Build rules
all: $(BUILD_DIR) $(TARGET_TOOLS)

$(BUILD_DIR):
	$(V)mkdir -p $(BUILD_DIR)
$(BUILD_DIR)/%:
	$(V)mkdir -p $@

$(BUILD_DIR)/__%: %.c
	$(V_CC)${CC} ${CCFLAGS} $< -o $@


# Tools usage
mkfs: $(BUILD_DIR) $(MKFS)
	./$(MKFS) Readme
	mv $(NANOFS_IMAGE) $(NANOFS_DIR)/$(NANOFS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)
