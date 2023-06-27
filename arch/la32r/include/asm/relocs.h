/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * MIPS Relocations
 *
 * Copyright (c) 2017 Imagination Technologies Ltd.
 */

#ifndef __ASM_MIPS_RELOCS_H__
#define __ASM_MIPS_RELOCS_H__

#define R_LARCH_NONE				0
#define R_LARCH_32				1
#define R_LARCH_SOP_PUSH_PCREL			22       //
#define R_LARCH_SOP_PUSH_ABSOLUTE		23       //
#define R_LARCH_SOP_PUSH_GPREL			25       //
#define R_LARCH_SOP_PUSH_PLT_PCREL		29
#define R_LARCH_SOP_SUB				32       //
#define R_LARCH_SOP_SL				33       //
#define R_LARCH_SOP_SR				34       //
#define R_LARCH_SOP_ADD				35       //
#define R_LARCH_SOP_POP_32_S_10_12		40   //
#define R_LARCH_SOP_POP_32_S_10_16_S2		42
#define R_LARCH_SOP_POP_32_S_5_20		43       //
#define R_LARCH_SOP_POP_32_S_0_10_10_16_S2	45

#endif /* __ASM_MIPS_RELOCS_H__ */
