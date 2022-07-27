#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <fdtdec.h>
#include <spi.h>
#include <asm/io.h>
#include <clk.h>

// 寄存器偏移
#define SPCR                (0)     // 控制寄存器
#define SPSR                (1)     // 状态寄存器
#define FIFO                (2)     // 发送的数据寄存器，与接收数据寄存器的偏移相同
#define SPER                (3)     // 外部寄存器
#define SFC_PARAM           (4)     // 参数控制寄存器
#define SFC_SOFTCS          (5)     // 片选控制寄存器
#define SFC_TIMING          (6)     // 时序控制寄存器

// 寄存器SPCR中的位域
#define LS1C_SPI_SPCR_SPIE_BIT              (7)
#define LS1C_SPI_SPCR_SPIE_MASK             (0x01 << LS1C_SPI_SPCR_SPIE_BIT)
#define LS1C_SPI_SPCR_SPE_BIT               (6)
#define LS1C_SPI_SPCR_SPE_MASK              (0x01 << LS1C_SPI_SPCR_SPE_BIT)
#define LS1C_SPI_SPCR_CPOL_BIT              (3)
#define LS1C_SPI_SPCR_CPOL_MASK             (0x01 << LS1C_SPI_SPCR_CPOL_BIT)
#define LS1C_SPI_SPCR_CPHA_BIT              (2)
#define LS1C_SPI_SPCR_CPHA_MASK             (0x01 << LS1C_SPI_SPCR_CPHA_BIT)
#define LS1C_SPI_SPCR_SPR_BIT               (0)
#define LS1C_SPI_SPCR_SPR_MASK              (0x03 << LS1C_SPI_SPCR_SPR_BIT)

// 寄存器SPSR中的位域
#define LS1C_SPI_SPSR_SPIF_BIT              (7)
#define LS1C_SPI_SPSR_SPIF_MASK             (0x01 << LS1C_SPI_SPSR_SPIF_BIT)
#define LS1C_SPI_SPSR_WCOL_BIT              (6)
#define LS1C_SPI_SPSR_WCOL_MASK             (0x01 << LS1C_SPI_SPSR_WCOL_BIT)
#define LS1C_SPI_SPSR_WFFULL_BIT            (3)
#define LS1C_SPI_SPSR_WFFULL_MASK           (0x01 << LS1C_SPI_SPSR_WFFULL_BIT)
#define LS1C_SPI_SPSR_WFEMPTY_BIT           (2)
#define LS1C_SPI_SPSR_WFEMPTY_MASK          (0x01 << LS1C_SPI_SPSR_WFEMPTY_BIT)
#define LS1C_SPI_SPSR_RFFULL_BIT            (1)
#define LS1C_SPI_SPSR_RFFULL_MASK           (0x01 << LS1C_SPI_SPSR_RFFULL_BIT)
#define LS1C_SPI_SPSR_RFEMPTY_BIT           (0)
#define LS1C_SPI_SPSR_RFEMPTY_MASK          (0x01 << LS1C_SPI_SPSR_RFEMPTY_BIT)

// 寄存器SPER中的位域
#define LS1C_SPI_SPER_SPRE_BIT              (0)
#define LS1C_SPI_SPER_SPRE_MASK             (0x3 << LS1C_SPI_SPER_SPRE_BIT)

// 寄存器SFC_SOFTCS的位域
#define LS1C_SPI_SFC_SOFTCS_CSN_BIT         (4)
#define LS1C_SPI_SFC_SOFTCS_CSN_MASK        (0x0f << LS1C_SPI_SFC_SOFTCS_CSN_BIT)
#define LS1C_SPI_SFC_SOFTCS_CSEN_BIT        (0)
#define LS1C_SPI_SFC_SOFTCS_CSEN_MASK       (0x0f << LS1C_SPI_SFC_SOFTCS_CSEN_BIT)

#define LS1C_SPI_SFC_PARAM_MEMORY_EN_BIT    (0)
#define LS1C_SPI_SFC_PARAM_MEMORY_EN_MASK   (0x01 << LS1C_SPI_SFC_PARAM_MEMORY_EN_BIT)

// 发送超时的门限值
#define LS1C_SPI_TX_TIMEOUT                 (20000)

/*
struct loongson_spi_platdata {
    struct loongson_spi_regs* regs;
};
*/

struct loongson_spi_priv {
    void __iomem *base;
    ulong bus_clk_rate;
    uint cur_hz;
    struct clk clk;
};

static int loongson_spi_claim_bus(struct udevice *slave)
{
	struct udevice *bus = dev_get_parent(slave);
	struct loongson_spi_priv *priv = dev_get_priv(bus);

	debug("%s\n", __func__);
    clrsetbits_8(priv->base + SPSR, 0, LS1C_SPI_SPSR_SPIF_MASK | LS1C_SPI_SPSR_WCOL_MASK);
        clrsetbits_8(priv->base + SFC_PARAM, LS1C_SPI_SFC_PARAM_MEMORY_EN_MASK, 0);
        clrsetbits_8(priv->base + SPCR, 0, LS1C_SPI_SPCR_SPE_MASK);
        
        return 0;
}

static int loongson_spi_release_bus(struct udevice *slave)
{
	struct udevice *bus = dev_get_parent(slave);
	struct loongson_spi_priv *priv = dev_get_priv(bus);

	debug("%s\n", __func__);

        clrsetbits_8(priv->base + SPCR, LS1C_SPI_SPCR_SPE_MASK, 0);
        clrsetbits_8(priv->base + SFC_PARAM, 0, LS1C_SPI_SFC_PARAM_MEMORY_EN_MASK);
        
        return 0;
}

static u8 loongson_spi_read_fifo(struct udevice *bus)
{
    struct loongson_spi_priv *priv = dev_get_priv(bus);
    return readb(priv->base + FIFO);
}

static void loongson_spi_write_fifo(struct udevice *bus, u8 val)
{
    struct loongson_spi_priv *priv = dev_get_priv(bus);
    writeb(val, priv->base + FIFO);
}

static int loongson_spi_xfer(struct udevice *slave, unsigned int bitlen,
                            const void *dout, void *din, unsigned long flags)
{
    
    struct udevice *bus = dev_get_parent(slave);
	struct loongson_spi_priv *priv = dev_get_priv(bus);
        struct dm_spi_slave_platdata *slave_plat = dev_get_parent_platdata(slave);
    int len = bitlen / 8;
    const u8* tx_buf = dout;
    u8* rx_buf = din;

    if (flags & SPI_XFER_BEGIN) {
        setbits_8(priv->base + SFC_SOFTCS, 0xFF);
        clrbits_8(priv->base + SFC_SOFTCS, (1 << (slave_plat->cs + 4)));
    }

    // Wait for FIFO empty
    while (!readb(priv->base + SPSR) & LS1C_SPI_SPSR_RFEMPTY_MASK)
        loongson_spi_read_fifo(bus);
        
    while (len > 0) {
        u8 rx_val;
        if (tx_buf) {
            loongson_spi_write_fifo(bus, *tx_buf);
            tx_buf++;
        } else {
            loongson_spi_write_fifo(bus, 0);
        }
        len--;
        while (readb(priv->base + SPSR) & LS1C_SPI_SPSR_RFEMPTY_MASK); // Wait for RX done
        rx_val = loongson_spi_read_fifo(bus);
        if (rx_buf) {
            *rx_buf = rx_val;
            rx_buf++;
        }
    }
    
    if (flags & SPI_XFER_END) {
        clrsetbits_8(priv->base + SFC_SOFTCS, 0, 0xFF);
    }

    return 0;
}


static const u8 rdiv[12] = {0, 1, 4, 2, 3, 5, 6, 7, 8, 9, 10, 11};

static int loongson_spi_set_speed(struct udevice* bus, uint hz) {
	struct loongson_spi_priv *priv = dev_get_priv(bus);
        u32 div, mbrdiv;

        if (priv->cur_hz == hz)
            return 0;

        div = DIV_ROUND_UP(priv->bus_clk_rate, hz);
        if (div < 2)
		return -EINVAL;

        if (div & (div - 1))
		mbrdiv = fls(div);
	else
		mbrdiv = fls(div) - 1;

        mbrdiv -= 1;
        if (mbrdiv > 11)
		return -EINVAL;
        
        clrsetbits_8(priv->base + SPCR, LS1C_SPI_SPCR_SPR_MASK, rdiv[mbrdiv] & 0b11);
        clrsetbits_8(priv->base + SPER, LS1C_SPI_SPER_SPRE_MASK, (rdiv[mbrdiv] >> 2) & 0b11);

        priv->cur_hz = hz;
        return 0;
}

static int loongson_spi_set_mode(struct udevice* bus, uint mode) {
    struct loongson_spi_priv *priv = dev_get_priv(bus);
    u8 clrb = 0, setb = 0;
    if (mode & SPI_CPOL)
        setb |= LS1C_SPI_SPCR_CPOL_MASK;
    else
        clrb |= LS1C_SPI_SPCR_CPOL_MASK;

    if (mode & SPI_CPHA)
        setb |= LS1C_SPI_SPCR_CPHA_MASK;
    else
        clrb |= LS1C_SPI_SPCR_CPHA_MASK;

    clrsetbits_8(priv->base + SPCR, clrb, setb);

    return 0;
}

static int loongson_spi_probe(struct udevice *dev) {
    struct loongson_spi_priv *priv = dev_get_priv(dev);
    unsigned long clk_rate;
    int ret;

    priv->base = dev_remap_addr(dev);
	if (!priv->base)
		return -EINVAL;


	/* enable clock */
	ret = clk_get_by_index(dev, 0, &priv->clk);
	if (ret < 0)
		return ret;

	clk_rate = clk_get_rate(&priv->clk);
	if (!clk_rate) {
		ret = -EINVAL;
		goto clk_err;
	}

	priv->bus_clk_rate = clk_rate;

clk_err:
	clk_disable(&priv->clk);
	clk_free(&priv->clk);

	return ret;
}

static const struct dm_spi_ops loongson_spi_ops = {
    .claim_bus     = loongson_spi_claim_bus,
    .release_bus   = loongson_spi_release_bus,
    .xfer          = loongson_spi_xfer,
    .set_speed     = loongson_spi_set_speed,
    .set_mode      = loongson_spi_set_mode,
};

static const struct udevice_id loongson_spi_ids[] = {
	{ .compatible = "loongson,spi" },
	{}
};

U_BOOT_DRIVER(loongson_spi) = {
	.name	= "loongson_spi",
	.id	= UCLASS_SPI,
	.of_match   = loongson_spi_ids,
	.ops	    = &loongson_spi_ops,
	.priv_auto_alloc_size	= sizeof(struct loongson_spi_priv),
	.probe   	= loongson_spi_probe,
};
