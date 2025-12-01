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
