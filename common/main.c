// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/* #define	DEBUG	*/

#include <common.h>
#include <autoboot.h>
#include <cli.h>
#include <console.h>
#include <version.h>

/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
__weak void show_boot_progress(int val) {}

static void run_preboot_environment_command(void)
{
#ifdef CONFIG_PREBOOT
	char *p;

	p = env_get("preboot");
	if (p != NULL) {
		int prev = 0;

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			prev = disable_ctrlc(1); /* disable Ctrl-C checking */

		run_command_list(p, -1, 0);

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			disable_ctrlc(prev);	/* restore Ctrl-C checking */
	}
#endif /* CONFIG_PREBOOT */
}

/* We come here after U-Boot is initialised and ready to process commands */
void main_loop(void)
{
	int dbg_cnt = 0;
	const char *s;
	printf("main_loop: %d\n", ++dbg_cnt);

	bootstage_mark_name(BOOTSTAGE_ID_MAIN_LOOP, "main_loop");
	printf("main_loop: %d\n", ++dbg_cnt);

	if (IS_ENABLED(CONFIG_VERSION_VARIABLE))
		env_set("ver", version_string);  /* set version variable */

	printf("main_loop: %d\n", ++dbg_cnt);
	cli_init();
	printf("main_loop: %d\n", ++dbg_cnt);

	run_preboot_environment_command();
	printf("main_loop: %d\n", ++dbg_cnt);

	if (IS_ENABLED(CONFIG_UPDATE_TFTP))
		update_tftp(0UL, NULL, NULL);
	printf("main_loop: %d\n", ++dbg_cnt);

	s = bootdelay_process();
	printf("main_loop: %d\n", ++dbg_cnt);
	if (cli_process_fdt(&s))
		cli_secure_boot_cmd(s);
	printf("main_loop: %d %s\n", ++dbg_cnt,s);

	autoboot_command(s);
	printf("main_loop: %d", ++dbg_cnt);

	cli_loop();
	printf("main_loop: %d", ++dbg_cnt);
	panic("No CLI available");
}
