/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 1985 MIPS Computer Systems, Inc.
 * Copyright (C) 1994, 95, 99, 2003 by Ralf Baechle
 * Copyright (C) 1990 - 1992, 1999 Silicon Graphics, Inc.
 * Copyright (C) 2011 Wind River Systems,
 *   written by Ralf Baechle <ralf@linux-mips.org>
 */
#ifndef _ASM_REGDEF_H
#define _ASM_REGDEF_H

#include <asm/sgidefs.h>

/*
 * Symbolic register names for 32 bit ABI
 */
#define zero	$r0	/* wired zero */
#define ra	$r1	/* return address */
#define tp	$r2
#define sp	$r3	/* stack pointer */
#define v0	$r4	/* return value - caller saved */
#define v1	$r5
#define a0	$r4	/* argument registers */
#define a1	$r5
#define a2	$r6
#define a3	$r7
#define a4	$r8
#define a5	$r9
#define a6	$r10
#define a7	$r11
#define t0	$r12	/* caller saved */
#define t1	$r13
#define t2	$r14
#define t3	$r15
#define t4	$r16
#define t5	$r17
#define t6	$r18
#define t7	$r19
#define t8	$r20
#define x0	$r21
#define fp	$r22	/* frame pointer */
#define s0	$r23	/* callee saved */
#define s1	$r24
#define s2	$r25
#define s3	$r26
#define s4	$r27
#define s5	$r28
#define s6	$r29
#define s7	$r30
#define s8	$r31

#endif /* _ASM_REGDEF_H */
