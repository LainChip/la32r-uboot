// SPDX-License-Identifier: GPL-2.0+
/*
 * Wishbone SD Card Controller IP Core driver
 * https://github.com/mczerski/SD-card-controller
 *
 * Copyright (C) 2013 Marek Czerski <ma.czerski@gmail.com>
 * Copyright (C) 2021 Yunhao Tian <t123yh@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

// #define DEBUG

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <mmc.h>
#include <clk.h>
#include <asm/io.h>
#include <asm/cache.h>
#include <linux/kernel.h>

// Register space
#define OCSDC_ARGUMENT           0x00
#define OCSDC_COMMAND            0x04
#define OCSDC_RESPONSE_1         0x08
#define OCSDC_RESPONSE_2         0x0c
#define OCSDC_RESPONSE_3         0x10
#define OCSDC_RESPONSE_4         0x14
#define OCSDC_DATA_TIMEOUT       0x18
#define OCSDC_CONTROL 			 0x1C
#define OCSDC_CMD_TIMEOUT        0x20
#define OCSDC_CLOCK_DIVIDER      0x24
#define OCSDC_SOFTWARE_RESET     0x28
#define OCSDC_POWER_CONTROL      0x2C
#define OCSDC_CAPABILITY         0x30
#define OCSDC_CMD_INT_STATUS     0x34
#define OCSDC_CMD_INT_ENABLE     0x38
#define OCSDC_DAT_INT_STATUS     0x3C
#define OCSDC_DAT_INT_ENABLE     0x40
#define OCSDC_BLOCK_SIZE         0x44
#define OCSDC_BLOCK_COUNT        0x48
#define OCSDC_DST_SRC_ADDR       0x60

//command register
#define OCSDC_COMMAND_NO_RESP		0x0
#define OCSDC_COMMAND_RESP_48		0x1
#define OCSDC_COMMAND_RESP_136		0x2
#define OCSDC_COMMAND_BUSY_CHECK	0x4
#define OCSDC_COMMAND_CRC_CHECK		0x8
#define OCSDC_COMMAND_INDEX_CHECK	0x10
#define OCSDC_COMMAND_DATA_READ		0x20
#define OCSDC_COMMAND_DATA_WRITE	0x40
#define OCSDC_COMMAND_INDEX(x) ((x) << 8)

// OCSDC_CMD_INT_STATUS bits
#define OCSDC_CMD_INT_STATUS_CC   0x0001
#define OCSDC_CMD_INT_STATUS_EI   0x0002
#define OCSDC_CMD_INT_STATUS_CTE  0x0004
#define OCSDC_CMD_INT_STATUS_CCRC 0x0008
#define OCSDC_CMD_INT_STATUS_CIE  0x0010

// OCSDC_CMD_INT_STATUS bits
#define OCSDC_DATA_INT_STATUS_CC     0x0001
#define OCSDC_DATA_INT_STATUS_EI     0x0002
#define OCSDC_DATA_INT_STATUS_CTE    0x0002
#define OCSDC_DATA_INT_STATUS_CCRCE  0x0008
#define OCSDC_DATA_INT_STATUS_CFE    0x0010


struct ocsdc_mmc_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

struct ocsdc_mmc_priv {
        fdt_addr_t iobase;
	uint32_t clk_freq;
	int clk_div;
};

static inline u32 ocsdc_read(struct ocsdc_mmc_priv *dev, int offset)
{
	return readl((volatile void*)(dev->iobase + offset));
}

static inline void ocsdc_write(struct ocsdc_mmc_priv *dev, int offset, u32 data)
{
	writel(data, (volatile void*)(dev->iobase + offset));
}

static void ocsdc_set_timeout(struct ocsdc_mmc_priv * dev)
{
	const uint timeout_us = 200000; // 200ms
	uint clk_freq = (dev->clk_freq) / (2 * (dev->clk_div + 1));
	uint timeout_reg = timeout_us * (clk_freq / 1000000);
	if (timeout_reg > 0xFFFF)
		timeout_reg = 0xFFFF;

	debug("ocsdc_set_timeout 0x%x\n", timeout_reg);
	ocsdc_write(dev, OCSDC_CMD_TIMEOUT, 0xFFFF);
	ocsdc_write(dev, OCSDC_DATA_TIMEOUT, 0xFFFFFF);
}

static void ocsdc_set_clock(struct ocsdc_mmc_priv * dev, uint32_t clock)
{
	dev->clk_div = DIV_ROUND_UP(dev->clk_freq, (2 * clock)) - 1;

	debug("ocsdc_set_clock %d, div %d\n", clock, dev->clk_div);
	//software reset
	ocsdc_write(dev, OCSDC_SOFTWARE_RESET, 1);
	//set clock devider
	ocsdc_write(dev, OCSDC_CLOCK_DIVIDER, dev->clk_div);
	//clear software reset
	ocsdc_write(dev, OCSDC_SOFTWARE_RESET, 0);

	ocsdc_set_timeout(dev);
}

static uint ocsdc_get_voltage(struct ocsdc_mmc_priv * dev) {
	u32 v = ocsdc_read(dev, OCSDC_POWER_CONTROL);
	uint voltage = 0;
	if (v >= 1650 && v <= 1950)
		voltage |= MMC_VDD_165_195;
	if (v >= 2000 && v <= 2100)
		voltage |= MMC_VDD_20_21;
	if (v >= 2100 && v <= 2200)
		voltage |= MMC_VDD_21_22;
	if (v >= 2200 && v <= 2300)
		voltage |= MMC_VDD_22_23;
	if (v >= 2300 && v <= 2400)
		voltage |= MMC_VDD_23_24;
	if (v >= 2400 && v <= 2500)
		voltage |= MMC_VDD_24_25;
	if (v >= 2500 && v <= 2600)
		voltage |= MMC_VDD_25_26;
	if (v >= 2600 && v <= 2700)
		voltage |= MMC_VDD_26_27;
	if (v >= 2700 && v <= 2800)
		voltage |= MMC_VDD_27_28;
	if (v >= 2800 && v <= 2900)
		voltage |= MMC_VDD_28_29;
	if (v >= 2900 && v <= 3000)
		voltage |= MMC_VDD_29_30;
	if (v >= 3000 && v <= 3100)
		voltage |= MMC_VDD_30_31;
	if (v >= 3100 && v <= 3200)
		voltage |= MMC_VDD_31_32;
	if (v >= 3200 && v <= 3300)
		voltage |= MMC_VDD_32_33;
	if (v >= 3300 && v <= 3400)
		voltage |= MMC_VDD_33_34;
	if (v >= 3400 && v <= 3500)
		voltage |= MMC_VDD_34_35;
	if (v >= 3500 && v <= 3600)
		voltage |= MMC_VDD_35_36;

	debug("ocsdc_get_voltage 0x%x\n", voltage);
	return voltage;
}

static void ocsdc_set_buswidth(struct ocsdc_mmc_priv * dev, uint width) {
	if (width == 4)
		ocsdc_write(dev, OCSDC_CONTROL, 1);
	else if (width == 1)
		ocsdc_write(dev, OCSDC_CONTROL, 0);
}

static int ocsdc_mmc_set_ios(struct udevice *dev)
{
    struct ocsdc_mmc_priv *priv = dev_get_priv(dev);
	struct mmc *mmc = mmc_get_mmc_dev(dev);
	debug("ocsdc_mmc_set_ios called\n");

	debug("ocsdc bus_width: %x, clock: %d\n", mmc->bus_width, mmc->clock);
        ocsdc_set_buswidth(priv, mmc->bus_width);

	/* Set clock speed */
	if (mmc->clock) {
		ocsdc_set_clock(priv, mmc->clock);
        }

        return 0;
}

static void ocsdc_setup_data_xfer(struct ocsdc_mmc_priv * dev, struct mmc_cmd *cmd, struct mmc_data *data) {
	//invalidate cache
	if (data->flags & MMC_DATA_READ) {
                debug("ocsdc: read dma to %X\n", data->dest);
		flush_dcache_range((unsigned long)data->dest,
			(unsigned long)data->dest+data->blocksize*data->blocks);
		ocsdc_write(dev, OCSDC_DST_SRC_ADDR, virt_to_phys(data->dest));
	} else if (data->flags & MMC_DATA_WRITE) {
                debug("ocsdc: write dma from %X\n", data->src);
		flush_dcache_range((unsigned long)data->src,
			(unsigned long)data->src+data->blocksize*data->blocks);
		ocsdc_write(dev, OCSDC_DST_SRC_ADDR, virt_to_phys(data->src));
	} else {
            debug("Warning: invalid flag %d\n", data->flags);
        }
        debug("ocsdc: block size = %d, block count = %d\n", data->blocksize, data->blocks);
	ocsdc_write(dev, OCSDC_BLOCK_SIZE, data->blocksize-1);
	ocsdc_write(dev, OCSDC_BLOCK_COUNT, data->blocks-1);
}

static int ocsdc_finish(struct ocsdc_mmc_priv * dev, struct mmc_cmd *cmd) {

	int retval = 0;
	while (1) {
		int int_stat = ocsdc_read(dev, OCSDC_CMD_INT_STATUS);
		if (int_stat & OCSDC_CMD_INT_STATUS_EI) {
			//clear interrupts
			ocsdc_write(dev, OCSDC_CMD_INT_STATUS, 0);
			debug("ocsdc_finish: cmd %d, status %x\n", cmd->cmdidx, int_stat);
			retval = -1;
			break;
		}
		else if (int_stat & OCSDC_CMD_INT_STATUS_CC) {
			//clear interrupts
			ocsdc_write(dev, OCSDC_CMD_INT_STATUS, 0);
			//get response
			cmd->response[0] = ocsdc_read(dev, OCSDC_RESPONSE_1);
			if (cmd->resp_type & MMC_RSP_136) {
				cmd->response[1] = ocsdc_read(dev, OCSDC_RESPONSE_2);
				cmd->response[2] = ocsdc_read(dev, OCSDC_RESPONSE_3);
				cmd->response[3] = ocsdc_read(dev, OCSDC_RESPONSE_4);
			}
			debug("ocsdc_finish:  %d ok\n", cmd->cmdidx);
			retval = 0;

			break;
		}
		//else if (!(int_stat & OCSDC_CMD_INT_STATUS_CIE)) {
		//	debug("ocsdc_finish: cmd %d no exec %x\n", cmd->cmdidx, r2);
		//}
	}
	return retval;
}

static int ocsdc_data_finish(struct ocsdc_mmc_priv * dev) {
	int status;

    while ((status = ocsdc_read(dev, OCSDC_DAT_INT_STATUS)) == 0);
    ocsdc_write(dev, OCSDC_DAT_INT_STATUS, 0);

    if (status & OCSDC_DATA_INT_STATUS_CC) {
    	debug("ocsdc_data_finish: ok\n");
    	return 0;
    }
    else {
    	debug("ocsdc_data_finish: status %x\n", status);
    	return -1;
    }
}

static int ocsdc_mmc_send_cmd(struct udevice *udev, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	struct ocsdc_mmc_priv *dev = dev_get_priv(udev);
	int command = OCSDC_COMMAND_INDEX(cmd->cmdidx);
	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136)
			command |= OCSDC_COMMAND_RESP_136;
		else {
			command |= OCSDC_COMMAND_RESP_48;
		}
	}
	if (cmd->resp_type & MMC_RSP_BUSY)
		command |= OCSDC_COMMAND_BUSY_CHECK;
	if (cmd->resp_type & MMC_RSP_CRC)
		command |= OCSDC_COMMAND_CRC_CHECK;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		command |= OCSDC_COMMAND_INDEX_CHECK;

	if (data && ((data->flags & MMC_DATA_READ) || ((data->flags & MMC_DATA_WRITE))) && data->blocks) {
		if (data->flags & MMC_DATA_READ)
			command |= OCSDC_COMMAND_DATA_READ;
		if (data->flags & MMC_DATA_WRITE)
			command |= OCSDC_COMMAND_DATA_WRITE;
		ocsdc_setup_data_xfer(dev, cmd, data);
	}

	debug("ocsdc_send_cmd %04x\n", command);

	ocsdc_write(dev, OCSDC_COMMAND, command);
	ocsdc_write(dev, OCSDC_ARGUMENT, cmd->cmdarg);

	if (ocsdc_finish(dev, cmd) < 0) return -1;
	if (data && data->blocks) return ocsdc_data_finish(dev);
	else return 0;
}

static int ocsdc_init(struct ocsdc_mmc_priv *dev)
{
	//disable all interrupts
	ocsdc_write(dev, OCSDC_CMD_INT_ENABLE, 0);
	ocsdc_write(dev, OCSDC_DAT_INT_ENABLE, 0);
	//clear all interrupts
	ocsdc_write(dev, OCSDC_CMD_INT_STATUS, 0);
	ocsdc_write(dev, OCSDC_DAT_INT_STATUS, 0);
	//set clock to maximum (devide by 2)
	ocsdc_set_clock(dev, dev->clk_freq/2);

	return 0;
}

static const struct dm_mmc_ops ocsdc_mmc_ops = {
	.send_cmd	= ocsdc_mmc_send_cmd,
	.set_ios	= ocsdc_mmc_set_ios,
	.get_cd		= NULL,
        .get_wp         = NULL,
        .send_init_stream = NULL
};

static int ocsdc_mmc_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct ocsdc_mmc_plat *plat = dev_get_platdata(dev);
	struct ocsdc_mmc_priv *priv = dev_get_priv(dev);
	struct mmc_config *cfg = &plat->cfg;
        struct clk clk;
	int ret;
        debug("ocsdc probe %s\n", dev->name);

	cfg->name = dev->name;

	ret = clk_get_by_index(dev, 0, &clk);
	if (ret) {
		debug("ocsdc: clk_get_by_index() failed: %d\n", ret);
		return ret;
	}

        priv->clk_freq = clk_get_rate(&clk);
        priv->iobase = dev_read_addr(dev);
        cfg->f_min = priv->clk_freq/64; /*maximum clock division by 64 */
		cfg->f_max = priv->clk_freq/2; /*minimum clock division by 2 */
        cfg->voltages = ocsdc_get_voltage(priv);
        cfg->host_caps = MMC_MODE_4BIT;
        cfg->b_max = 0x100;
        upriv->mmc = &plat->mmc;

        ocsdc_init(priv);
        debug("ocsdc successfully probed\n");
        return 0;
}

static int ocsdc_mmc_bind(struct udevice *dev)
{
	struct ocsdc_mmc_plat *plat = dev_get_platdata(dev);

        debug("ocsdc bind\n");
	return mmc_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id ocsdc_mmc_ids[] = {
	{ .compatible = "opencores,ocsdc" },
	{ }
};

U_BOOT_DRIVER(ocsdc_mmc_drv) = {
	.name		= "ocsdc_mmc",
	.id		= UCLASS_MMC,
	.of_match	= ocsdc_mmc_ids,
	.bind		= ocsdc_mmc_bind,
	.probe		= ocsdc_mmc_probe,
	.ops		= &ocsdc_mmc_ops,
	.platdata_auto_alloc_size = sizeof(struct ocsdc_mmc_plat),
	.priv_auto_alloc_size = sizeof(struct ocsdc_mmc_priv),
};
