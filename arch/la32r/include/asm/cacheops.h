/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Cache operations for the cache instruction.
 *
 * (C) Copyright 1996, 97, 99, 2002, 03 Ralf Baechle
 * (C) Copyright 1999 Silicon Graphics, Inc.
 */
#ifndef	__ASM_CACHEOPS_H
#define	__ASM_CACHEOPS_H

#ifndef __ASSEMBLY__

static inline void mips_cache(int op, const volatile void *addr)
{
	__asm__ __volatile__("cacop	%0, %1" : : "i"(op), "R" (*(unsigned char *)(addr)));
}

#define MIPS32_WHICH_ICACHE                    0x0
#define MIPS32_FETCH_AND_LOCK                  0x7

#define ICACHE_LOAD_LOCK (MIPS32_WHICH_ICACHE | (MIPS32_FETCH_AND_LOCK << 2))

/* Prefetch and lock instructions into cache */
static inline void icache_lock(void *func, size_t len)
{
	// TODO: WHAT IS THIS FUNCTION FOR.
	// int i, lines = ((len - 1) / ARCH_DMA_MINALIGN) + 1;

	// for (i = 0; i < lines; i++) {
	// 	asm volatile (" cache %0, %1(%2)"
	// 		      : /* No Output */
	// 		      : "I" ICACHE_LOAD_LOCK,
	// 			"n" (i * ARCH_DMA_MINALIGN),
	// 			"r" (func)
	// 		      : /* No Clobbers */);
	// }
}
#endif /* !__ASSEMBLY__ */

/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
// #define INDEX_INVALIDATE_I      0x00
// #define INDEX_WRITEBACK_INV_D   0x01
// #define INDEX_LOAD_TAG_I	0x04
// #define INDEX_LOAD_TAG_D	0x05
// #define INDEX_STORE_TAG_I	0x08
// #define INDEX_STORE_TAG_D	0x09
// #if defined(CONFIG_CPU_LOONGSON2)
// #define HIT_INVALIDATE_I	0x00
// #else
// #define HIT_INVALIDATE_I	0x10
// #endif
// #define HIT_INVALIDATE_D	0x11
// #define HIT_WRITEBACK_INV_D	0x15
#define Cache_I				0x00
#define Cache_D				0x01
#define Cache_V				0x02
#define Cache_S				0x03

#define INDEX_INVALIDATE		0x08
#define INDEX_WRITEBACK_INV		0x08
#define HIT_INVALIDATE			0x10
#define HIT_WRITEBACK_INV		0x10

#define INDEX_INVALIDATE_I		(Cache_I | INDEX_INVALIDATE)
#define INDEX_WRITEBACK_INV_D	(Cache_D | INDEX_WRITEBACK_INV)
#define HIT_INVALIDATE_I		(Cache_I | HIT_INVALIDATE)
#define HIT_INVALIDATE_D		(Cache_D | HIT_INVALIDATE)
#define HIT_WRITEBACK_INV_D		(Cache_D | HIT_WRITEBACK_INV)

#endif	/* __ASM_CACHEOPS_H */
