#ifndef _TRAP_H_
#define _TRAP_H_

#ifndef __ASSEMBLER__

struct Trapframe {
	/* Saved main processor registers. */
	unsigned long regs[32];

	/* Saved special registers. */
    unsigned long crmd;
    unsigned long prmd;
    unsigned long euen;
    unsigned long ectl;
    unsigned long estat;
    unsigned long era;
    unsigned long badv;
    unsigned long eentry;
    unsigned long tlbidx;
    unsigned long tlbehi;
    unsigned long tlbelo0;
    unsigned long tlbelo1;
    unsigned long asid;
    unsigned long pgdl;
    unsigned long pgdh;
    unsigned long pgd;
    unsigned long cpuid;
    unsigned long save0;
    unsigned long save1;
    unsigned long save2;
    unsigned long save3;
    unsigned long tid;
    unsigned long tcfg;
    unsigned long tval;
    unsigned long ticlr;
    unsigned long llbctl;
    unsigned long tlbrentry;
    unsigned long dmw0;
    unsigned long dmw1;
};

#endif /* !__ASSEMBLER__ */

#define TF_REG0 0
#define TF_REG1 ((TF_REG0) + 4)
#define TF_REG2 ((TF_REG1) + 4)
#define TF_REG3 ((TF_REG2) + 4)
#define TF_REG4 ((TF_REG3) + 4)
#define TF_REG5 ((TF_REG4) + 4)
#define TF_REG6 ((TF_REG5) + 4)
#define TF_REG7 ((TF_REG6) + 4)
#define TF_REG8 ((TF_REG7) + 4)
#define TF_REG9 ((TF_REG8) + 4)
#define TF_REG10 ((TF_REG9) + 4)
#define TF_REG11 ((TF_REG10) + 4)
#define TF_REG12 ((TF_REG11) + 4)
#define TF_REG13 ((TF_REG12) + 4)
#define TF_REG14 ((TF_REG13) + 4)
#define TF_REG15 ((TF_REG14) + 4)
#define TF_REG16 ((TF_REG15) + 4)
#define TF_REG17 ((TF_REG16) + 4)
#define TF_REG18 ((TF_REG17) + 4)
#define TF_REG19 ((TF_REG18) + 4)
#define TF_REG20 ((TF_REG19) + 4)
#define TF_REG21 ((TF_REG20) + 4)
#define TF_REG22 ((TF_REG21) + 4)
#define TF_REG23 ((TF_REG22) + 4)
#define TF_REG24 ((TF_REG23) + 4)
#define TF_REG25 ((TF_REG24) + 4)
#define TF_REG26 ((TF_REG25) + 4)
#define TF_REG27 ((TF_REG26) + 4)
#define TF_REG28 ((TF_REG27) + 4)
#define TF_REG29 ((TF_REG28) + 4)
#define TF_REG30 ((TF_REG29) + 4)
#define TF_REG31 ((TF_REG30) + 4)

#define TF_CRMD      ((TF_REG31) + 4)
#define TF_PRMD      ((TF_CRMD) + 4)
#define TF_EUEN      ((TF_PRMD) + 4)
#define TF_ECTL      ((TF_EUEN) + 4)
#define TF_ESTAT     ((TF_ECTL) + 4)
#define TF_ERA       ((TF_ESTAT) + 4)
#define TF_BADV      ((TF_ERA) + 4)
#define TF_EENTRY    ((TF_BADV) + 4)
#define TF_TLBIDX    ((TF_EENTRY) + 4)
#define TF_TLBEHI    ((TF_TLBIDX) + 4)
#define TF_TLBELO0   ((TF_TLBEHI) + 4)
#define TF_TLBELO1   ((TF_TLBELO0) + 4)
#define TF_ASID      ((TF_TLBELO1) + 4)
#define TF_PGDL      ((TF_ASID) + 4)
#define TF_PGDH      ((TF_PGDL) + 4)
#define TF_PGD       ((TF_PGDH) + 4)
#define TF_CPUID     ((TF_PGD) + 4)
#define TF_SAVE0     ((TF_CPUID) + 4)
#define TF_SAVE1     ((TF_SAVE0) + 4)
#define TF_SAVE2     ((TF_SAVE1) + 4)
#define TF_SAVE3     ((TF_SAVE2) + 4)
#define TF_TID       ((TF_SAVE3) + 4)
#define TF_TCFG      ((TF_TID) + 4)
#define TF_TVAL      ((TF_TCFG) + 4)
#define TF_TICLR     ((TF_TVAL) + 4)
#define TF_LLBCTL    ((TF_TICLR) + 4)
#define TF_TLBRENTRY ((TF_LLBCTL) + 4)
#define TF_DMW0      ((TF_TLBRENTRY) + 4)
#define TF_DMW1      ((TF_DMW0) + 4)

#endif /* _TRAP_H_ */
