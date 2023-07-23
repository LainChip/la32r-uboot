#ifndef __TRIVIALMIPS_NSCSCC_CONFIG_H
#define __TRIVIALMIPS_NSCSCC_CONFIG_H

/* BootROM + MIG is pretty smart. DDR and Cache initialized */
#define CONFIG_SKIP_LOWLEVEL_INIT

// #define DEBUG

/*--------------------------------------------
 * CPU configuration
 */
/* CPU Timer rate */
#define CONFIG_SYS_MIPS_TIMER_FREQ	150000000

/* Cache Configuration */
#define CONFIG_SYS_MIPS_CACHE_MODE	CONF_CM_CACHABLE_NONCOHERENT

/* Time Measuring */
#define TRIVIALMIPS_NSCSCC_TIMER_ADDR		0x9fd0e000

/*----------------------------------------------------------------------
 * Memory Layout
 */

/* SDRAM Configuration (for final code, data, stack, heap) */
#define CONFIG_SYS_SDRAM_BASE		0xA0000000
#define CONFIG_SYS_SDRAM_SIZE		0x04000000	/* 128 Mbytes */
#define CONFIG_SYS_INIT_SP_ADDR		\
	(CONFIG_SYS_SDRAM_BASE + CONFIG_SYS_SDRAM_SIZE - 0x1000)

#define CONFIG_SYS_MEMTEST_START    0xA0000000
#define CONFIG_SYS_MEMTEST_END      0x04000000

#define CONFIG_SYS_MALLOC_LEN		(256 << 10)
#define CONFIG_SYS_MONITOR_BASE		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_LOAD_ADDR		0xA0000000 /* default load address */

/*----------------------------------------------------------------------
 * Commands
 */
//#define CONFIG_SYS_LONGHELP		/* undef to save memory */
//#define CONFIG_CMDLINE_EDITING

/*------------------------------------------------------------
 * Console Configuration
 */
#define CONFIG_SYS_CBSIZE		1024 /* Console I/O Buffer Size   */
#define CONFIG_SYS_MAXARGS		16   /* max number of command args*/


// serial configuration
#define CONFIG_CONS_INDEX 1
#define CONFIG_SYS_NS16550_COM1 0x9fe40000
#define CONFIG_SYS_NS16550_CLK 100000000
#define CONFIG_SYS_NS16550_IER 0


/* -------------------------------------------------
 * Environment
 */
//Disable persistent environment variable storage
#define CONFIG_ENV_IS_NOWHERE   1 


#define CONFIG_ENV_SIZE		0x4000
#define CONFIG_EXTRA_ENV_SETTINGS \
    "autoload=no\0" \
    "serverip=192.168.1.141\0" \
    "ipaddr=192.168.1.70\0" \
    "netmask=255.255.255.0\0" \
    "ethaddr=98:5d:ad:11:22:33\0"



/* ---------------------------------------------------------------------
 * Board boot configuration
 */

#define CONFIG_TIMESTAMP	/* Print image info with timestamp */


/* Flash */
#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT   1
#define CONFIG_SYS_MAX_FLASH_SECT           64


#endif /* __TRIVIALMIPS_NSCSCC_CONFIG_H */

