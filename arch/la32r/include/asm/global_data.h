/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2002-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

#ifndef	__ASM_GBL_DATA_H
#define __ASM_GBL_DATA_H

#include <asm/regdef.h>

/* Architecture-specific global data */
struct arch_global_data {
};

#include <asm-generic/global_data.h>

#define DECLARE_GLOBAL_DATA_PTR     register volatile gd_t *gd asm ("$r21")

#endif /* __ASM_GBL_DATA_H */
