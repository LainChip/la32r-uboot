# SPDX-License-Identifier: GPL-2.0+

dtb-$(CONFIG_TARGET_LA32RSOC_DEMO) += la32rsoc_demo.dtb
dtb-$(CONFIG_TARGET_LA32RMEGA_DEMO) += la32rmega_demo.dtb

targets += $(dtb-y)

# Add any required device tree compiler flags here
DTC_FLAGS +=

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
