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
and	sp, sp, t0		# force 16 byte alignment
add.w	x0, sp, zero			# save gd pointer
#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	li.w	t2, CONFIG_VAL(SYS_MALLOC_F_LEN)
	sub.w	sp, sp, t2		# reserve space for early malloc
	and		sp, sp, t0		# force 16 byte alignment
#endif
	add.w	fp, sp, zero

	/* Clear gd */
	add.w	x0, x0, zero
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

// #if defined(CONFIG_ROM_EXCEPTION_VECTORS)
	.org 0x380
1:	b	1b
# 	/*
# 	 * Exception vector entry points. When running from ROM, an exception
# 	 * cannot be handled. Halt execution and transfer control to debugger,
# 	 * if one is attached.
# 	 */
# 	.org 0x200
# 	/* TLB refill, 32 bit task */
# 	uhi_mips_exception

# 	.org 0x280
# 	/* XTLB refill, 64 bit task */
# 	uhi_mips_exception

# 	.org 0x300
# 	/* Cache error exception */
# 	uhi_mips_exception

# 	.org 0x380
# 	/* General exception */
# 	uhi_mips_exception

# 	.org 0x400
# 	/* Catch interrupt exceptions */
# 	uhi_mips_exception

# 	.org 0x480
# 	/* EJTAG debug exception */
# 1:	b	1b
# 	 nop

# 	.org 0x500
// #endif

reset:
# 	/* Hang if this isn't the first CPU in the system */
# 2:	beqz	t0, 4f
# 	 nop
# 3:	wait
# 	b	3b
# 	 nop

	/* Init CSR Status */
	// mfc0	t0, CP0_STATUS
	// and	t0, ST0_IMPL
	// or	t0, ST0_BEV | ST0_ERL | STATUS_SET
	// mtc0	t0, CP0_STATUS
	li.w      t0, 0x0c000380 
    csrwr   t0, csr_eentry 

    li.w      t1, 0xa0000009  // uncacheed window
    csrwr   t1, csr_dmw0 
    li.w      t1, 0x00000009  // cacheed window, cache attribute is set after 
    csrwr   t1, csr_dmw1
    li.w      t1, 0x10 
    li.w      t2, 0x18 
    csrxchg t1, t2, csr_crmd  //enable address translation

//     bl      locate
// uncached:
//     nop
// locate:
//     la      s0, uncached 
//     sub.w   s0, ra, s0

	// TODO: check what device is in *0xbfd0f010
    // li.w      t0, 0x33333333 
    // li.w      a0, 0xbfd0f010 
    // st.w    t0, a0, 0  

	// No need to check because we only support LA32R
	/*
	 * Check whether CP0 Config1 is implemented. If not continue
	 * with legacy Watch register initialization.
	 */
	// mfc0	t0, CP0_CONFIG
	// bgez	t0, wr_legacy
	//  nop
	/*
	 * Check WR bit in CP0 Config1 to determine if Watch registers
	 * are implemented.
	 */
	// mfc0	t0, CP0_CONFIG, 1
	// andi	t0, (1 << 3)
	// beqz	t0, wr_done
	//  nop
	/* Clear Watch Status bits and disable watch exceptions */
	// li	t1, 0x7		# Clear I, R and W conditions
	// init_wr	0
	// init_wr	1
	// init_wr	2
	// init_wr	3
	// init_wr	4
	// init_wr	5
	// init_wr	6
	// init_wr	7
	// b	wr_done
	//  nop
// wr_legacy:
// 	MTC0	zero, CP0_WATCHLO
// 	mtc0	zero, CP0_WATCHHI

// wr_done:

	/* Clear WP, IV and SW interrupts */
	// mtc0	zero, CP0_CAUSE


	/* Clear timer interrupt (CP0_COUNT cleared on branch to 'reset') */
	// mtc0	zero, CP0_COMPARE

// #ifndef CONFIG_SKIP_LOWLEVEL_INIT
// 	mfc0	t0, CP0_CONFIG
// 	and	t0, t0, MIPS_CONF_IMPL
// 	or	t0, t0, CONF_CM_UNCACHED
// 	mtc0	t0, CP0_CONFIG
// 	ehb
// #endif
// #ifdef CONFIG_MIPS_CM
// 	PTR_LA	t9, mips_cm_map
// 	jalr	t9
// 	 nop
// #endif
// #ifdef CONFIG_MIPS_INIT_STACK_IN_SRAM
// 	/* Set up initial stack and global data */
// 	setup_stack_gd

#ifdef CONFIG_DEBUG_UART
	/* Earliest point to set up debug uart */
	// PTR_LA	t9, debug_uart_init
	// jalr	t9
	bl		debug_uart_init
	// nop
#endif
// #endif

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#ifdef CONFIG_SYS_MIPS_CACHE_INIT_RAM_LOAD
	/* Initialize any external memory */
	// PTR_LA	t9, lowlevel_init
	// jalr	t9
	bl		lowlevel_init
	//  nop
#endif

	/* Initialize caches... */
	// PTR_LA	t9, mips_cache_reset
	// jalr	t9
	bl		lal1_cache_reset
	//  nop

#ifndef CONFIG_SYS_MIPS_CACHE_INIT_RAM_LOAD
	/* Initialize any external memory */
	// PTR_LA	t9, lowlevel_init
	// jalr	t9
	bl		lowlevel_init
	//  nop
#endif
#else
	// la t1, kseg1_kseg0 #####
	// li.w t2, ~0x20000000  ###
	// and t9, t1, t2    #kseg1 -> kseg0
	// jr t9              ###
	// nop                #####

kseg1_kseg0:
#endif

#ifndef CONFIG_MIPS_INIT_STACK_IN_SRAM
	/* Set up initial stack and global data */
	setup_stack_gd

# ifdef CONFIG_DEBUG_UART
	/* Earliest point to set up debug uart */
	// PTR_LA	t9, debug_uart_init
	// jalr	t9
	bl		debug_uart_init
	//  nop
# endif
#endif

	add.w	a0, zero, zero		# a0 <-- boot_flags = 0
	// PTR_LA	t9, board_init_f
	add.w	ra, zero,zero
	b	board_init_f
	// jr	t9


	END(_start)
