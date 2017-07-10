
# Directory that building target install in.
INSTALL_DIR = $(INSTALL_PATH)/firmware

# Build Firmware target with make rules in makefile.mk
all:
	$(MAKE) -f makefile.mk all CORE=c6xdsp
	cp build/bin/dvr_rdk_c6xdsp_$(PROFILE_c6xdsp)_$(DDR_SUFFIX)_$(LINUX_SUFFIX).xe674 \
		$(INSTALL_DIR)/dvr_rdk_fw_c6xdsp_$(DDR_SUFFIX)_$(LINUX_SUFFIX).xe674

clean:
	$(MAKE) -f makefile.mk clean CORE=c6xdsp

.PHONY: all clean
