# SPDX-License-Identifier: GPL-2.0+

head-y := arch/la32r/cpu/start.o

ifeq ($(CONFIG_SPL_BUILD),y)
ifneq ($(CONFIG_SPL_START_S_PATH),)
head-y := $(CONFIG_SPL_START_S_PATH:"%"=%)/start.o
endif
endif

libs-y += arch/la32r/cpu/
libs-y += arch/la32r/lib/

PLATFORM_CPPFLAGS += $(patsubst %,-I$(srctree)/%include,$(machdirs))

PLATFORM_CPPFLAGS += $(arch-y) $(tune-y) $(cflags-y)
