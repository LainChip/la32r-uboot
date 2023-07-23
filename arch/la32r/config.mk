# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

32bit-emul		:= elf32loongarch
64bit-emul		:= elf64loongarch
32bit-bfd		:= elf32-tradlittlemips
64bit-bfd		:= elf64-tradlittlemips
PLATFORM_CPPFLAGS	+=
PLATFORM_LDFLAGS	+=

PLATFORM_CPPFLAGS	+= -mabi=ilp32s
PLATFORM_LDFLAGS	+= -m $(32bit-emul)
OBJCOPYFLAGS		+= -O $(32bit-bfd)
CONFIG_STANDALONE_LOAD_ADDR	?= 0xa0200000

PLATFORM_ELFENTRY = "__start"
PLATFORM_ELFFLAGS += -B la32r $(OBJCOPYFLAGS)

#
# From Linux arch/mips/Makefile
#
# GCC uses -G 0 -mabicalls -fpic as default.  We don't want PIC in the kernel
# code since it only slows down the whole thing.  At some point we might make
# use of global pointer optimizations but their use of $28 conflicts with
# the current pointer optimization.
#
# The DECStation requires an ECOFF kernel for remote booting, other MIPS
# machines may also.  Since BFD is incredibly buggy with respect to
# crossformat linking we rely on the elf2ecoff tool for format conversion.
#
# cflags-y			+= -G 0 -mno-abicalls -fno-pic -pipe
# cflags-y			+= -msoft-float
# LDFLAGS_vmlinux		+= -G 0 -static -n -nostdlib
# MODFLAGS			+= -mlong-calls
#
ifndef CONFIG_SPL_BUILD
OBJCOPYFLAGS			+= -j .data.reloc -j .dtb.init.rodata
LDFLAGS_FINAL			+= --emit-relocs
endif

# PLATFORM_CPPFLAGS		+= -G 0 -mno-abicalls -fno-pic
PLATFORM_CPPFLAGS		+= -G 0 -fno-pic
PLATFORM_CPPFLAGS		+= -msoft-float
PLATFORM_LDFLAGS		+= -G 0 -static -n -nostdlib
PLATFORM_RELFLAGS		+= -ffunction-sections -fdata-sections
LDFLAGS_FINAL			+= --gc-sections
OBJCOPYFLAGS			+= -j .text -j .rodata -j .data -j .u_boot_list

LDFLAGS_STANDALONE		+= --gc-sections
