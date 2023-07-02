# 构建命令
export DEVICE_TREE=la32rsoc_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rsoc_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S && cp u-boot ../qemu_work/u-boot

# Megasoc 构建命令
export DEVICE_TREE=la32rmega_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rmega_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S
loongarch32r-linux-gnusf-objcopy ./u-boot -O binary u-boot.bin

# Uboot 启动 MOS 命令
fatload mmc 0 0x80000000 mos.bin
go 0x80011550
