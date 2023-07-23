/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Startup Code for LA32R CPU-core
 *
 *  Copyright (c) 2023	Wang Zhe <dofingert@gmail.com>
 */

#include<asm-offsets.h>
#include<config.h>
#include<asm/asm.h>
#include<asm/regdef.h>
#include<asm/mipsregs.h>
#include<asm/traps.h>

#ifndef CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + \
				CONFIG_SYS_INIT_SP_OFFSET)
#endif

#ifdef CONFIG_32BIT
# define MIPS_RELOC	3
# define STATUS_SET	0
#endif

#ifdef CONFIG_64BIT
# ifdef CONFIG_SYS_LITTLE_ENDIAN
#  define MIPS64_R_INFO(ssym, r_type3, r_type2, r_type) \
	(((r_type) << 24) | ((r_type2) << 16) | ((r_type3) << 8) | (ssym))
# else
#  define MIPS64_R_INFO(ssym, r_type3, r_type2, r_type) \
	((r_type) | ((r_type2) << 8) | ((r_type3) << 16) | (ssym) << 24)
# endif
# define MIPS_RELOC	MIPS64_R_INFO(0x00, 0x00, 0x12, 0x03)
# define STATUS_SET	ST0_KX
#endif

	# .set noreorder;

	# .macro init_wr sel
	# MTC0	zero, CP0_WATCHLO,\sel
	# mtc0	t1, CP0_WATCHHI,\sel
	# mfc0	t0, CP0_WATCHHI,\sel
	# bgez	t0, wr_done
	#  nop
	# .endm

	# .macro uhi_mips_exception
	# add.w	k0, t9		# preserve t9 in k0
	# add.w	k1, a0		# preserve a0 in k1
	# li	t9, 15		# UHI exception operation
	# li	a0, 0		# Use hard register context
	# sdbbp	1		# Invoke UHI operation
	# .endm

.macro setup_stack_gd
li.w	t0, -16
// PTR_LI	t1, CONFIG_SYS_INIT_SP_ADDR
li.w	t1, CONFIG_SYS_INIT_SP_ADDR
and		sp, t1, t0		# force 16 byte alignment
addi.w	sp, sp, -GD_SIZE		# reserve space for gd
and		sp, sp, t0		# force 16 byte alignment
add.w	x0, sp, zero			# save gd pointer
#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	li.w	t2, CONFIG_VAL(SYS_MALLOC_F_LEN)
	sub.w	sp, sp, t2		# reserve space for early malloc
	and		sp, sp, t0		# force 16 byte alignment
#endif
	add.w	fp, sp, zero

	/* Clear gd */
	add.w	t0, x0, zero
1:
	// PTR_S	zero, 0(t0)
	st.w	zero, t0, 0
	addi.w	t0, t0, 4
	blt	t0, t1, 1b
	// PTR_ADDIU t0, PTRSIZE

#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	// PTR_S	sp, GD_MALLOC_BASE(k0)	# gd->malloc_base offset
	st.w	sp, x0, GD_MALLOC_BASE
#endif
	.endm

ENTRY(_start)
	/* U-Boot entry point */
	#  mtc0	zero, CP0_COUNT	# clear cp0 count for most accurate boot timing
	# csrwr   $r12, csr_ticlr
	b	reset

	.org 0x380
	csrwr     t0, csr_save0
	csrwr     t1, csr_save1
	lu12i.w   t0, -131072
	pcaddu12i t1, 0
	and       t0, t1, t0
	
	// save context, jump to print trapframe function.
	st.w $r0, t0, TF_REG0
	st.w $r1, t0, TF_REG1
	st.w $r2, t0, TF_REG2
	st.w $r3, t0, TF_REG3
	st.w $r4, t0, TF_REG4
	st.w $r5, t0, TF_REG5
	st.w $r6, t0, TF_REG6
	st.w $r7, t0, TF_REG7
	st.w $r8, t0, TF_REG8
	st.w $r9, t0, TF_REG9
	st.w $r10, t0, TF_REG10
	st.w $r11, t0, TF_REG11

	csrrd     $r10, csr_save0
	csrrd     $r11, csr_save1
	// r12 - t0, r13 - t1 is in csr_save0, csr_save1
	st.w $r10, t0, TF_REG12
	st.w $r11, t0, TF_REG13

	st.w $r14, t0, TF_REG14
	st.w $r15, t0, TF_REG15
	st.w $r16, t0, TF_REG16
	st.w $r17, t0, TF_REG17
	st.w $r18, t0, TF_REG18
	st.w $r19, t0, TF_REG19
	st.w $r20, t0, TF_REG20
	st.w $r21, t0, TF_REG21
	st.w $r22, t0, TF_REG22
	st.w $r23, t0, TF_REG23
	st.w $r24, t0, TF_REG24
	st.w $r25, t0, TF_REG25
	st.w $r26, t0, TF_REG26
	st.w $r27, t0, TF_REG27
	st.w $r28, t0, TF_REG28
	st.w $r29, t0, TF_REG29
	st.w $r30, t0, TF_REG30
	st.w $r31, t0, TF_REG31
	// save context, jump to print trapframe function.
	csrrd	t1, csr_crmd
	st.w    t1, t0, TF_CRMD
	csrrd	t1, csr_prmd
	st.w    t1, t0, TF_PRMD
	csrrd	t1, csr_euen
	st.w    t1, t0, TF_EUEN
	csrrd	t1, csr_ectl
	st.w    t1, t0, TF_ECTL
	csrrd	t1, csr_estat
	st.w    t1, t0, TF_ESTAT
	csrrd	t1, csr_era
	st.w    t1, t0, TF_ERA
	csrrd	t1, csr_badv
	st.w    t1, t0, TF_BADV
	csrrd	t1, csr_eentry
	st.w    t1, t0, TF_EENTRY
	csrrd	t1, csr_tlbidx
	st.w    t1, t0, TF_TLBIDX
	csrrd	t1, csr_tlbehi
	st.w    t1, t0, TF_TLBEHI
	csrrd	t1, csr_tlbelo0
	st.w    t1, t0, TF_TLBELO0
	csrrd	t1, csr_tlbelo1
	st.w    t1, t0, TF_TLBELO1
	csrrd	t1, csr_asid
	st.w    t1, t0, TF_ASID
	csrrd	t1, csr_pgdl
	st.w    t1, t0, TF_PGDL
	csrrd	t1, csr_pgdh
	st.w    t1, t0, TF_PGDH
	csrrd	t1, csr_pgd
	st.w    t1, t0, TF_PGD
	csrrd	t1, csr_cpuid
	st.w    t1, t0, TF_CPUID
	csrrd	t1, csr_save0
	st.w    t1, t0, TF_SAVE0
	csrrd	t1, csr_save1
	st.w    t1, t0, TF_SAVE1
	csrrd	t1, csr_save2
	st.w    t1, t0, TF_SAVE2
	csrrd	t1, csr_save3
	st.w    t1, t0, TF_SAVE3
	csrrd	t1, csr_tid
	st.w    t1, t0, TF_TID
	csrrd	t1, csr_tcfg
	st.w    t1, t0, TF_TCFG
	csrrd	t1, csr_tval
	st.w    t1, t0, TF_TVAL
	csrrd	t1, csr_ticlr
	st.w    t1, t0, TF_TICLR
	csrrd	t1, csr_llbctl
	st.w    t1, t0, TF_LLBCTL
	csrrd	t1, csr_tlbrentry
	st.w    t1, t0, TF_TLBRENTRY
	csrrd	t1, csr_dmw0
	st.w    t1, t0, TF_DMW0
	csrrd	t1, csr_dmw1
	st.w    t1, t0, TF_DMW1
	add.w   a0, t1, zero
	b       do_reserved

reset:
	li.w    t0, 0x9c000380
    csrwr   t0, csr_eentry
	li.w    t0, 0x1c000380
	csrwr   t0, csr_tlbrentry

    li.w      t1, 0xa0000009  // cacheed window, cache attribute is set after 
    csrwr   t1, csr_dmw0 
    li.w      t1, 0x00000009  // uncacheed window, temporary
    csrwr   t1, csr_dmw1
    li.w      t1, 0x10 
    li.w      t2, 0x18 
    csrxchg t1, t2, csr_crmd  //enable address translation

	la		t0, cseg
	jr		t0
cseg:
    li.w    t1, 0x80000009  // uncacheed window
    csrwr   t1, csr_dmw1
	

// 	/* Set up initial stack and global data */
	setup_stack_gd

#ifdef CONFIG_DEBUG_UART
	/* Earliest point to set up debug uart */
	bl		debug_uart_init
#endif

	/* Initialize caches... */
	// PTR_LA	t9, mips_cache_reset
	// jalr	t9
	bl		lal1_cache_reset
    li.w      t1, 0xa0000019  // cacheed window, set cache attribute
    csrwr   t1, csr_dmw0
	//  nop

	// TODO: lowlevel_init
	/* Initialize any external memory */
	// PTR_LA	t9, lowlevel_init
	// jalr	t9
	// bl		lowlevel_init
	//  nop

	add.w	a0, zero, zero		# a0 <-- boot_flags = 0
	// PTR_LA	t9, board_init_f
	add.w	ra, zero,zero
	b	board_init_f
	// jr	t9


END(_start)

