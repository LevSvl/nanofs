include $(NANOFS_DIR)/Common.mk

CROSS_COMPILE := 

CC = ${CROSS_COMPILE}gcc
AS = ${CROSS_COMPILE}as
LD = ${CROSS_COMPILE}ld
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump

TOOLS_LIST = $(shell ls $(CURDIR) | grep -v *.mk)
TOOL_NAME = $(subst build-,,$(subst clean-,,$(subst use-,,$(subst tools-,,$@))))

export

# Build rules
all: $(BUILD_DIR) $(TARGET_TOOLS)

tools-build-all:
	$(foreach tool,$(TOOLS_LIST),$(MAKE) -C $(tool) -f $(tool).mk build)
tools-clean-all:
	$(foreach tool,$(TOOLS_LIST),$(MAKE) -C $(tool) -f $(tool).mk clean)

tools-build-%:
	$(MAKE) -C $(TOOL_NAME) -f $(TOOL_NAME).mk build

tools-clean-%:
	$(MAKE) -C $(TOOL_NAME) -f $(TOOL_NAME).mk clean

tools-use-%:
	$(MAKE) -C $(TOOL_NAME) -f $(TOOL_NAME).mk use
