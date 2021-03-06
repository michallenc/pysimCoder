all: allfiles lib install
reduced: files lib install

LIB = libpyblk.a
RTPYINC = ../include
SRCALL = $(wildcard *.c)
OBJ = $(SRCALL:.c=.o)
CWD = $(shell pwd)

NUTTX_EXPORT = $(PWD)/../nuttx-export

ifndef NUTTX_EXPORT
$(warning Specify NUTTX_EXPORT)
$(warning make_rtw NUTTX_EXPORT=/path/to/nuttx-export)
$(error NUTTX_EXPORT not defined, cannot continue)
endif

include $(NUTTX_EXPORT)/scripts/Make.defs

NUTTX_INCLUDES = -isystem $(NUTTX_EXPORT)/include

SYSTEM_LIBS += --start-group $(LDLIBS) $(EXTRA_LIBS) --end-group
ELF_MODULE_LIBS = --start-group $(EXTRA_LIBS) --end-group

LDFLAGS += -L $(NUTTX_EXPORT)/libs

TARGET_ARCH_FLAGS ?= $(ARCHCFLAGS) $(ARCHCPUFLAGS) \
	-mlong-calls -fno-common -DWITHOUT_MLOCK

DEFAULT_OPT_OPTS ?= $(ARCHOPTIMIZATION)

ifndef OPT_OPTS
OPT_OPTS = $(DEFAULT_OPT_OPTS)
endif

LD_SCRIPT = $(NUTTX_EXPORT)/scripts/$(LDNAME)

LDFLAGS += -T $(LD_SCRIPT)

LDFLAGS  += --entry=__start -nostartfiles -nodefaultlibs

ELF_FILE_LDSCRIPT ?= $(wildcard $(NUTTX_EXPORT)/scripts/gnu-elf.ld)

INCLUDES += $(NUTTX_INCLUDES)

CFLAGS = $(TARGET_ARCH_FLAGS) $(ARCHWARNINGS) $(OPT_OPTS) $(INCLUDES)

CXXFLAGS = $(TARGET_ARCH_FLAGS) $(ARCHCXXFLAGS) $(ARCHWARNINGSXX) $(OPT_OPTS) $(INCLUDES)

OBJEX = $(SRC:.c=.o)
DEFINES = -D PLOTTER_SCRIPT=\"$(CWD)/scope.py\"
CC_FLAGS = -c $(DBG) -I$(RTPYINC) $(CFLAGS) 
CC_FLAGS_SCP = -c $(DBG) -I$(RTPYINC) $(CFLAGS) $(DEFINES)
CC_FLAGS_FMU = -c $(DBG) -I$(RTPYINC) $(CFLAGS) -Wall -DFMI_COSIMULATION \
                             -DSTANDALONE_XML_PARSER -DLIBXML_STATIC \
                              -I$(FMUDIR)/fmuinc/include -I$(FMUDIR)/fmuinc/parser -I$(FMUDIR)/fmuinc

allfiles:

files:
EXCLUDE = comedi_analog_input.c comedi_analog_output.c comedi_digital_input.c comedi_digital_output.c
SRC=$(filter-out $(EXCLUDE),$(SRCALL))

scope.o: scope.c
	$(CC) $(CC_FLAGS_SCP) $<

FMUinterface.o: FMUinterface.c
	$(CC) $(CC_FLAGS_FMU) $<

%.o: %.c
	$(CC) $(CC_FLAGS) $<

lib: $(OBJ)
	$(AR) $(LIB) $(OBJ)

install:
	mkdir -p ../lib
	mv $(LIB) ../lib

clean:
	rm -f $(LIB) $(OBJ)
