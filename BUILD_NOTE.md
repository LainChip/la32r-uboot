# qemu 构建命令
```sh
export DEVICE_TREE=la32rsoc_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rsoc_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S && cp u-boot ../qemu_work/u-boot
```

# chiplab 构建命令
```sh
export DEVICE_TREE=la32rsoc_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rsoc_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S && cp u-boot ../qemu_work/u-boot
```

# Megasoc 构建命令
```sh
export DEVICE_TREE=la32rmega_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
clear && make la32rmega_defconfig && make && loongarch32r-linux-gnusf-objdump -S u-boot > u-boot.S
loongarch32r-linux-gnusf-objcopy ./u-boot -O binary u-boot.bin
```

# Linux 启动命令
```sh
fatload mmc 0 0xa2000000 vmlinux
bootelf 0xa2000000 g console=ttyS0,230400 rdinit=/init 
bootelf 0xa2000000 g console=ttyS0,230400 rdinit=/sbin/init
```

# 编译-启动 MOS 命令
```sh
make && loongarch32r-linux-gnusf-objcopy ./target/mos ./target/mos.bin -O binary
fatload mmc 0 0xa0000000 mos.bin
fatload mmc 0 0xa3800000 fs.img
go 0xa0010b24
```
