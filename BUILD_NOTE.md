# 构建命令
export DEVICE_TREE=la32rsoc_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rsoc_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S && cp u-boot ../qemu_work/u-boot