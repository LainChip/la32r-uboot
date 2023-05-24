# 构建命令
export DEVICE_TREE=la32rsoc_demo
export ARCH=la32r
export CROSS_COMPILE=loongarch32r-linux-gnusf-
make megasoc_defconfig
make