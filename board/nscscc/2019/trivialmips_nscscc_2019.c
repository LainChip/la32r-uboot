
#include <common.h>
#include <linux/io.h>
#include <div64.h>

DECLARE_GLOBAL_DATA_PTR;

/* initialize the DDR Controller and PHY */
int dram_init(void)
{
    /* MIG IP block is smart and doesn't need SW
     * to do any init */
    gd->ram_size = CONFIG_SYS_SDRAM_SIZE;   /* in bytes */
    return 0;
}

#ifdef CONFIG_RESET_PHY_R
void reset_phy(void) {}
#endif


uint64_t notrace get_ticks(void)
{
	return readl((volatile uint32_t*) TRIVIALMIPS_NSCSCC_TIMER_ADDR);
}

ulong get_timer(ulong base)
{
        unsigned long now = timer_get_us();
	// do_div((&now), 1000);
        now /= 1000;
	return now - base;
}

unsigned long notrace timer_get_us(void)
{
	uint64_t ticks = get_ticks();
	do_div(ticks, 100);
	return ticks;
}

void __udelay(unsigned long usec)
{
        uint64_t tmp;

        tmp = timer_get_us() + usec; /* get current timestamp */

        while (timer_get_us() < tmp+1)     /* loop till event */
                 /*NOP*/;
}
