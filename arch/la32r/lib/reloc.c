// SPDX-License-Identifier: GPL-2.0+
/*
 * MIPS Relocation
 *
 * Copyright (c) 2017 Imagination Technologies Ltd.
 *
 * Relocation data, found in the .rel section, is generated by the la32r-relocs
 * tool & contains a record of all locations in the U-Boot binary that need to
 * be fixed up during relocation.
 *
 * The data is a sequence of unsigned integers, which are of somewhat arbitrary
 * size. This is achieved by encoding integers as a sequence of bytes, each of
 * which contains 7 bits of data with the most significant bit indicating
 * whether any further bytes need to be read. The least significant bits of the
 * integer are found in the first byte - ie. it somewhat resembles little
 * endian.
 *
 * Each pair of two integers represents a relocation that must be applied. The
 * first integer represents the type of relocation as a standard ELF relocation
 * type (ie. R_MIPS_*). The second integer represents the offset at which to
 * apply the relocation, relative to the previous relocation or for the first
 * relocation the start of the relocated .text section.
 *
 * The end of the relocation data is indicated when type R_MIPS_NONE (0) is
 * read, at which point no further integers should be read. That is, the
 * terminating R_MIPS_NONE reloc includes no offset.
 */

#include <common.h>
#include <asm/relocs.h>
#include <asm/sections.h>

/**
 * read_uint() - Read an unsigned integer from the buffer
 * @buf: pointer to a pointer to the reloc buffer
 *
 * Read one whole unsigned integer from the relocation data pointed to by @buf,
 * advancing @buf past the bytes encoding the integer.
 *
 * Returns: the integer read from @buf
 */
static unsigned long read_uint(uint8_t **buf)
{
	unsigned long val = 0;
	unsigned int shift = 0;
	uint8_t new;

	do {
		new = *(*buf)++;
		val |= (new & 0x7f) << shift;
		shift += 7;
	} while (new & 0x80);

	return val;
}
static int rela_stack_push(long stack_value, long *rela_stack, size_t *rela_stack_top)
{
	if (*rela_stack_top >= 16) {
		printf("rela_stack_top: %d\n",*rela_stack_top);
		panic("rela stack push overflow");
	}

	rela_stack[(*rela_stack_top)++] = stack_value;

	return 0;
}

static int rela_stack_pop(long *stack_value, long *rela_stack, size_t *rela_stack_top)
{
	if (*rela_stack_top == 0) {
		printf("rela_stack_top: %d\n",*rela_stack_top);
		panic("rela stack pop overflow");
	}

	*stack_value = rela_stack[--(*rela_stack_top)];

	return 0;
}
/**
 * apply_reloc() - Apply a single relocation
 * @type: the type of reloc (R_MIPS_*)
 * @addr: the address that the reloc should be applied to
 * @off: the relocation offset, ie. number of bytes we're moving U-Boot by
 * Apply a single relocation of type @type at @addr. This function is
 * intentionally simple, and does the bare minimum needed to fixup the
 * relocated U-Boot - in particular, it does not check for overflows.
 * 
 * @addend: the relocation addend
 *
 */
static void apply_reloc(unsigned int type, void *addr, long off, long sym, long* rela_stack, size_t *rela_stack_top)
{
	uint32_t u32;
	int op1,op2;

	switch (type) {
	case R_LARCH_32:
		*(uint32_t *)addr = sym + off;
		break;
	case R_LARCH_SOP_PUSH_PCREL:
		rela_stack_push(sym - (uint32_t)addr,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_PUSH_ABSOLUTE:
		rela_stack_push(sym,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_PUSH_GPREL:
		rela_stack_push(0,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_PUSH_PLT_PCREL:
		rela_stack_push(sym - (uint32_t)addr,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_SUB:
		rela_stack_pop(&op2,rela_stack,rela_stack_top);
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		rela_stack_push(op1 - op2,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_SL:
		rela_stack_pop(&op2,rela_stack,rela_stack_top);
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		rela_stack_push(op1 << op2,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_SR:
		rela_stack_pop(&op2,rela_stack,rela_stack_top);
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		rela_stack_push(op1 >> op2,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_ADD:
		rela_stack_pop(&op2,rela_stack,rela_stack_top);
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		rela_stack_push(op1 + op2,rela_stack,rela_stack_top);
		break;
	case R_LARCH_SOP_POP_32_S_10_12:
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		if ((op1 & ~0x7ff) &&
			(op1 & ~0x7ff) != ~0x7ff) {
			panic("1: op1 = 0x%x overflow! @0x%x\n",op1,addr);
		}
		*(uint32_t *)addr = ((*(uint32_t *)addr) & (~0x3ffc00)) | ((op1 & 0xfff) << 10);
		break;
	case R_LARCH_SOP_POP_32_S_10_16_S2:
		rela_stack_pop(&op1,rela_stack,rela_stack_top);

		/* check 4-aligned */
		if (op1 % 4) {
			panic("2: op1 = 0x%x unaligned! @0x%x\n",op1,addr);
		}
		op1 >>= 2;

		if ((op1 & ~0x7fff) &&
			(op1 & ~0x7fff) != ~0x7fff) {
			panic("3: op1 = 0x%x overflow! @0x%x\n",op1,addr);
		}
		(*(uint32_t *)addr) = ((*(uint32_t *)addr) & 0xfc0003ff) | ((op1 & 0xffff) << 10);
		break;
	case R_LARCH_SOP_POP_32_S_5_20:
		rela_stack_pop(&op1,rela_stack,rela_stack_top);
		if ((op1 & ~0x7ffff) &&
			(op1 & ~0x7ffff) != ~0x7ffff) {
			panic("4: op1 = 0x%x overflow! @0x%x\n",op1,addr);
		}
		*(uint32_t *)addr = ((*(uint32_t *)addr) & (~0x1ffffe0)) | ((op1 & 0xfffff) << 5);
		break;
	case R_LARCH_SOP_POP_32_S_0_10_10_16_S2:
		rela_stack_pop(&op1,rela_stack,rela_stack_top);

		/* check 4-aligned */
		if (op1 % 4) {
			panic("5: op1 = 0x%x unaligned! @0x%x\n",op1,addr);
		}
		op1 >>= 2;

		if ((op1 & ~0x1ffffff) &&
			(op1 & ~0x1ffffff) != ~0x1ffffff) {
			panic("6: op1 = 0x%x overflow! @0x%x\n",op1,addr);
		}
		*(uint32_t *)addr = ((*(uint32_t *)addr) & 0xfc000000)
		| ((op1 & 0x3ff0000) >> 16) | ((op1 & 0xffff) << 10);
		break;
	default:
		panic("Unhandled reloc type %u\n", type);
	}
}

/**
 * relocate_code() - Relocate U-Boot, generally from flash to DDR
 * @start_addr_sp: new stack pointer
 * @new_gd: pointer to relocated global data
 * @relocaddr: the address to relocate to
 *
 * Relocate U-Boot from its current location (generally in flash) to a new one
 * (generally in DDR). This function will copy the U-Boot binary & apply
 * relocations as necessary, then jump to board_init_r in the new build of
 * U-Boot. As such, this function does not return.
 */
void relocate_code(ulong start_addr_sp, gd_t *new_gd, ulong relocaddr)
{
	unsigned long addr, length, bss_len;
	uint8_t *buf, *bss_start;
	unsigned int type;
	long off, sym;
	long rela_stack[16];
	size_t rela_stack_top = 0;


	/*
	 * Ensure that we're relocating by an offset which is a multiple of
	 * 64KiB, ie. doesn't change the least significant 16 bits of any
	 * addresses. This allows us to discard R_MIPS_LO16 relocs, saving
	 * space in the U-Boot binary & complexity in handling them.
	 */
	off = relocaddr - (unsigned long)__text_start;
	if (off & 0xfff) {
		printf("off: 0x%08x\n", off);
		panic("Mis-aligned relocation\n");
	}

	/* Copy U-Boot to RAM */
	length = __image_copy_end - __text_start;
	memcpy((void *)relocaddr, __text_start, length);

	/* Now apply relocations to the copy in RAM */
	buf = __rel_start;
	addr = relocaddr;
	uint32_t *got = read_uint(&buf)+ off;
	uint32_t got_size = read_uint(&buf);
	printf("got located in %x, original %x, size %d\n",got,(char *)got - off,got_size);
	for(int i = 0 ; i < got_size ; i++) {
		got[i] += off;
	}
	while (true) {
		type = read_uint(&buf);
		if (type == 0)
			break;
		addr += read_uint(&buf) << 2;
		sym = read_uint(&buf);
		// printf("%x, %d, %d\n",addr,type,sym);
		apply_reloc(type, (void *)addr, off, sym, rela_stack, &rela_stack_top);
	}

	/* Ensure the icache is coherent */
	flush_cache(relocaddr, length);

	/* Clear the .bss section */
	bss_start = (uint8_t *)((unsigned long)__bss_start + off);
	bss_len = (unsigned long)&__bss_end - (unsigned long)__bss_start;
	memset(bss_start, 0, bss_len);

	/* Jump to the relocated U-Boot */
	asm volatile(
		       "add.w	$r3, %0, $r0\n"
		"	add.w	$r4, %1, $r0\n"
		"	add.w	$r5, %2, $r0\n"
		"	add.w	$r1, $r0, $r0\n"
		"	jr	%3"
		: /* no outputs */
		: "r"(start_addr_sp),
		  "r"(new_gd),
		  "r"(relocaddr),
		  "r"((unsigned long)board_init_r + off));

	/* Since we jumped to the new U-Boot above, we won't get here */
	unreachable();
}