/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 1994 - 1999, 2000, 03, 04 Ralf Baechle
 * Copyright (C) 2000, 2002  Maciej W. Rozycki
 * Copyright (C) 1990, 1999, 2000 Silicon Graphics, Inc.
 */
#ifndef _ASM_MACH_GENERIC_SPACES_H
#define _ASM_MACH_GENERIC_SPACES_H

#include <linux/const.h>

/*
 * This gives the physical RAM offset.
 */
#ifndef PHYS_OFFSET
#define PHYS_OFFSET		_AC(0, UL)
#endif

#ifdef CONFIG_KVM_GUEST
#define CAC_BASE		_AC(0x40000000, UL)
#else
#define CAC_BASE		_AC(0xa0000000, UL)
#endif
#ifndef IO_BASE
#define IO_BASE			_AC(0x80000000, UL)
#endif
#ifndef UNCAC_BASE
#define UNCAC_BASE		_AC(0x80000000, UL)
#endif

#ifndef MAP_BASE
#ifdef CONFIG_KVM_GUEST
#define MAP_BASE		_AC(0x60000000, UL)
#else
#define MAP_BASE		_AC(0xc0000000, UL)
#endif
#endif

/*
 * Memory above this physical address will be considered highmem.
 */
#ifndef HIGHMEM_START
#define HIGHMEM_START		_AC(0x20000000, UL)
#endif

/*
 * This handles the memory map.
 */
#ifndef PAGE_OFFSET
#define PAGE_OFFSET		(CAC_BASE + PHYS_OFFSET)
#endif

#ifndef FIXADDR_TOP
#ifdef CONFIG_KVM_GUEST
#define FIXADDR_TOP		((unsigned long)(long)(int)0x7ffe0000)
#else
#define FIXADDR_TOP		((unsigned long)(long)(int)0xfffe0000)
#endif
#endif

#endif /* __ASM_MACH_GENERIC_SPACES_H */
