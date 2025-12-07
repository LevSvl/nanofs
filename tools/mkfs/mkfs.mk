include $(NANOFS_DIR)/Common.mk

BUILD_DIR = $(realpath $(CURDIR))/build

INCLUDE = $(NANOFS_DIR)/include/nanofs
CCFLAGS += -DMKFS -I$(INCLUDE)

MKFS = $(BUILD_DIR)/__mkfs


build: $(BUILD_DIR) $(MKFS)

$(MKFS): mkfs.c
	$(V_CC)${CC} ${CCFLAGS} $< -o $@

$(BUILD_DIR):
	$(V)mkdir -p $(BUILD_DIR)
$(BUILD_DIR)/%:
	$(V)mkdir -p $@
$(BUILD_DIR)/__%: %.c
	$(V_CC)${CC} ${CCFLAGS} $< -o $@


use: $(BUILD_DIR) $(MKFS)
	$(MKFS) Readme
	mv $(NANOFS_IMAGE) $(NANOFS_DIR)/$(NANOFS_IMAGE)

clean:
	rm -rf $(BUILD_DIR)
