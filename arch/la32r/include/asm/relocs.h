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
#define R_LARCH_64				2
#define R_LARCH_RELATIVE			3
#define R_LARCH_COPY				4
#define R_LARCH_JUMP_SLOT			5
#define R_LARCH_TLS_DTPMOD32			6
#define R_LARCH_TLS_DTPMOD64			7
#define R_LARCH_TLS_DTPREL32			8
#define R_LARCH_TLS_DTPREL64			9
#define R_LARCH_TLS_TPREL32			10
#define R_LARCH_TLS_TPREL64			11
#define R_LARCH_IRELATIVE			12
#define R_LARCH_MARK_LA				20
#define R_LARCH_MARK_PCREL			21
#define R_LARCH_SOP_PUSH_PCREL			22
#define R_LARCH_SOP_PUSH_ABSOLUTE		23
#define R_LARCH_SOP_PUSH_DUP			24
#define R_LARCH_SOP_PUSH_GPREL			25
#define R_LARCH_SOP_PUSH_TLS_TPREL		26
#define R_LARCH_SOP_PUSH_TLS_GOT		27
#define R_LARCH_SOP_PUSH_TLS_GD			28
#define R_LARCH_SOP_PUSH_PLT_PCREL		29
#define R_LARCH_SOP_ASSERT			30
#define R_LARCH_SOP_NOT				31
#define R_LARCH_SOP_SUB				32
#define R_LARCH_SOP_SL				33
#define R_LARCH_SOP_SR				34
#define R_LARCH_SOP_ADD				35
#define R_LARCH_SOP_AND				36
#define R_LARCH_SOP_IF_ELSE			37
#define R_LARCH_SOP_POP_32_S_10_5		38
#define R_LARCH_SOP_POP_32_U_10_12		39
#define R_LARCH_SOP_POP_32_S_10_12		40
#define R_LARCH_SOP_POP_32_S_10_16		41
#define R_LARCH_SOP_POP_32_S_10_16_S2		42
#define R_LARCH_SOP_POP_32_S_5_20		43
#define R_LARCH_SOP_POP_32_S_0_5_10_16_S2	44
#define R_LARCH_SOP_POP_32_S_0_10_10_16_S2	45
#define R_LARCH_SOP_POP_32_U			46
#define R_LARCH_ADD8				47
#define R_LARCH_ADD16				48
#define R_LARCH_ADD24				49
#define R_LARCH_ADD32				50
#define R_LARCH_ADD64				51
#define R_LARCH_SUB8				52
#define R_LARCH_SUB16				53
#define R_LARCH_SUB24				54
#define R_LARCH_SUB32				55
#define R_LARCH_SUB64				56
#define R_LARCH_GNU_VTINHERIT			57
#define R_LARCH_GNU_VTENTRY			58

#endif /* __ASM_MIPS_RELOCS_H__ */
