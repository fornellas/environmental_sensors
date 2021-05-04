DEVICE = stm32f411ceu6
MAIN_MEMORY_BASE=0x08000000
STM_DFU_USB_VID=0x0483
STM_DFU_USB_PID=0xdf11
OOCD_INTERFACE = stlink-v2
OOCD_TARGET= stm32f4x

BUILT_SOURCES =
SUBDIRS = eglib .

##
## configure.ac dep
##

EXTRA_DIST = build-aux/libopencm3.mk

##
## opencm3 variables & rules
##

include $(OPENCM3_DIR)/mk/genlink-config.mk
# automake hides replaced variables, so we must redefine it here
LDSCRIPT = generated.$(DEVICE).ld
Q = $(AM_V_GEN)
include $(OPENCM3_DIR)/mk/genlink-rules.mk
CLEANFILES = $(LDSCRIPT)

.PHONY: libopencm3
libopencm3:
	$(MAKE) -C $(srcdir)/libopencm3/

.PHONY: libopencm3
clean-libopencm3:
	$(MAKE) -C $(srcdir)/libopencm3/ clean

clean-local: clean-libopencm3

all-recursive: libopencm3

##
## .bin rules
##

objcopy_verbose = $(objcopy_verbose_@AM_V@)
objcopy_verbose_ = $(objcopy_verbose_@AM_DEFAULT_V@)
objcopy_verbose_0 = @echo "  OBJCOPY " $@;

SUFFIXES = .elf.bin .bin

.elf.bin:
	$(objcopy_verbose)$(OBJCOPY) -O binary $< $@

##
## Common parameters
##

CFLAGS_COMMON =
CFLAGS_COMMON += -fno-common
CFLAGS_COMMON += -ffunction-sections -fdata-sections
CFLAGS_COMMON += $(CFLAGS_WARNINGS)
CFLAGS_COMMON += $(CFLAGS_EGLIB)
CFLAGS_COMMON += $(CFLAGS_OPENCM3)

LDFLAGS_COMMON =
LDFLAGS_COMMON += -T$(LDSCRIPT)
LDFLAGS_COMMON += -nostartfiles
LDFLAGS_COMMON += -specs=nano.specs
LDFLAGS_COMMON += -Wl,--gc-sections
LDFLAGS_COMMON += $(LDFLAGS_EGLIB)
LDFLAGS_COMMON += $(LDFLAGS_OPENCM3)

LDADD_COMMON =
LDADD_COMMON += $(LIBS_EGLIB)
LDADD_COMMON += $(LIBS_OPENCM3)

CPPFLAGS_COMMON =
CPPFLAGS_COMMON += $(CPPFLAGS_EGLIB)
CPPFLAGS_COMMON += $(CFLAGS_OPENCM3)

##
## PROGRAMS
##

noinst_PROGRAMS = main.elf
main_elf_CFLAGS = $(CFLAGS_COMMON)
main_elf_LDFLAGS = $(LDFLAGS_COMMON)
main_elf_LDADD = $(LDADD_COMMON)
main_elf_CPPFLAGS = $(CPPFLAGS_COMMON)
main_elf_SOURCES = main.c
EXTRA_main_elf_DEPENDENCIES = $(LDSCRIPT)
CLEANFILES += main.bin

##
## dfu
##

dfu_util_verbose = $(dfu_util_verbose_@AM_V@)
dfu_util_verbose_ = $(dfu_util_verbose_@AM_DEFAULT_V@)
dfu_util_verbose_0 = @echo "  DFU_UTIL " $@;

.PHONY: dfu
dfu: main.bin
	$(dfu_util_verbose)dfu-util --alt 0 --device $(STM_DFU_USB_VID):$(STM_DFU_USB_PID) --dfuse-address $(MAIN_MEMORY_BASE) --download main.bin --reset

##
## openocd
##

openocd_verbose = $(openocd_verbose_@AM_V@)
openocd_verbose_ = $(openocd_verbose_@AM_DEFAULT_V@)
openocd_verbose_0 = @echo "  OPENOCD " $@;

.PHONY: openocd
openocd:
	$(openocd_verbose)openocd --file interface/$(OOCD_INTERFACE).cfg --file target/$(OOCD_TARGET).cfg

gdb_verbose = $(gdb_verbose_@AM_V@)
gdb_verbose_ = $(gdb_verbose_@AM_DEFAULT_V@)
gdb_verbose_0 = @echo "  GDB " $@;

.PHONY: openocd_program
openocd_program: main.bin
	$(openocd_verbose)openocd --file interface/$(OOCD_INTERFACE).cfg --file target/$(OOCD_TARGET).cfg --command "program main.bin verify reset exit $(MAIN_MEMORY_BASE)"

##
## gdb
##

EXTRA_DIST += gdb.init

.PHONY: gdb
if HAS_GDB
gdb: $(noinst_PROGRAMS)
	$(gdb_verbose)$(GDB) --command=$(top_srcdir)/gdb.init main.elf
endif

##
## Version
##

EXTRA_DIST += $(top_srcdir)/.version
BUILT_SOURCES += $(top_srcdir)/.version

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version