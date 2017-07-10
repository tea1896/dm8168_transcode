
ifeq ($(DVR_RDK_BOARD_TYPE ), )
  #DVR_RDK_BOARD_TYPE := DM814X_TI_EVM
  DVR_RDK_BOARD_TYPE := DM816X_TI_EVM
endif

OS := Linux

# DVR_RDK_ROOT_PATH defined in file makfile.
dvr_rdk_BASE     := $(DVR_RDK_ROOT_PATH)
TI_SW_ROOT       := $(dvr_rdk_BASE)/ti_tools
dvr_rdk_PATH     := $(dvr_rdk_BASE)/dvr_rdk

# Code gen tools
CODEGEN_PATH_A8  := $(TI_SW_ROOT)/cgt_a8/arago/linux-devkit
CODEGEN_PATH_DSP := $(TI_SW_ROOT)/cgt_dsp/cgt6x_7_3_5/
CODEGEN_PREFIX   := $(CODEGEN_PATH_A8)/bin/arm-arago-linux-gnueabi-
CSTOOL_PREFIX    := arm-arago-linux-gnueabi-


# BIOS side tools
xdc_PATH         := $(TI_SW_ROOT)/xdc/xdctools_3_23_03_53
bios_PATH        := $(TI_SW_ROOT)/bios/bios_6_33_05_46
ipc_PATH         := $(TI_SW_ROOT)/ipc/ipc_1_24_03_32
fc_PATH          := $(TI_SW_ROOT)/framework_components/framework_components_3_22_02_08_patched
edma3lld_PATH    := $(TI_SW_ROOT)/edma3lld/edma3_lld_02_11_06_01

# Codecs
xdais_PATH         := $(TI_SW_ROOT)/xdais/xdais_7_22_00_03
hdvicplib_PATH     := $(TI_SW_ROOT)/ivahd_hdvicp/hdvicp20
h264dec_DIR        := $(TI_SW_ROOT)/codecs/REL.500.V.H264AVC.D.HP.IVAHD.02.00.08.00
h264enc_DIR        := $(TI_SW_ROOT)/codecs/REL.500.V.H264AVC.E.IVAHD.02.00.06.00

h264dec_PATH       := $(h264dec_DIR)/500.V.H264AVC.D.HP.IVAHD.02.00/IVAHD_001
h264enc_PATH       := $(h264enc_DIR)/500.V.H264AVC.E.IVAHD.02.00/IVAHD_001
jpegdec_DIR        := $(TI_SW_ROOT)/codecs/REL.500.V.MJPEG.D.IVAHD.01.00.06.00
jpegenc_DIR        := $(TI_SW_ROOT)/codecs/REL.500.V.MJPEG.E.IVAHD.01.00.04.00
jpegdec_PATH       := $(jpegdec_DIR)/500.V.MJPEG.D.IVAHD.01.00/IVAHD_001
jpegenc_PATH       := $(jpegenc_DIR)/500.V.MJPEG.E.IVAHD.01.00/IVAHD_001
mpeg4dec_DIR       := $(TI_SW_ROOT)/codecs/REL.500.V.MPEG4.D.IVAHD.01.00.11.00
mpeg4dec_PATH      := $(mpeg4dec_DIR)/500.V.MPEG4.D.ASP.IVAHD.01.00/IVAHD_001
mpeg4enc_DIR       := $(TI_SW_ROOT)/codecs/REL.500.V.MPEG4.E.SP.IVAHD.01.00.01.00
mpeg4enc_PATH      := $(mpeg4enc_DIR)/500.V.MPEG4.E.SP.IVAHD.01.00/IVAHD_001
mpeg2dec_DIR       := $(TI_SW_ROOT)/codecs/REL.500.V.MPEG2.D.IVAHD.01.00.12.00
mpeg2dec_PATH      := $(mpeg2dec_DIR)/500.V.MPEG2.D.IVAHD.01.00/IVAHD_001
watermark_lib_PATH := $(TI_SW_ROOT)/codecs/REL_WATERMARK_DECRYPT_IVAHD_00_04.zip

hdvpss_PATH      := $(TI_SW_ROOT)/hdvpss/dvr_rdk_hdvpss

# Audio framework (RPE) and Codecs
rpe_PATH         := $(TI_SW_ROOT)/rpe/remote-processor-execute
aaclcdec_PATH    := $(TI_SW_ROOT)/codecs/c674x_aaclcdec_01_41_00_00_elf
aaclcenc_PATH    := $(TI_SW_ROOT)/codecs/c674x_aaclcenc_01_00_01_00_elf_patched

# Linux side tools
syslink_PATH     := $(TI_SW_ROOT)/syslink/syslink_2_20_02_20
linuxdevkit_PATH := $(CODEGEN_PATH_A8)/arm-arago-linux-gnueabi
KERNELDIR        := $(TI_SW_ROOT)/linux_lsp/kernel/linux-dvr-rdk

memcpy_neon_PATH := $(dvr_rdk_PATH)/module/memcpy_neon

ROOTDIR := $(dvr_rdk_PATH)

# Default platform
ifeq ($(PLATFORM), )
  PLATFORM := ti816x-evm
ifeq ($(DVR_RDK_BOARD_TYPE),DM814X_TI_EVM)
  PLATFORM := ti814x-evm
endif
endif

###########################
# DDR_MEM and LINUX_MEM set.
###########################
# TI816X: 1G,256M; 2G,512M;
# TI814X: 512M,128M;

ifeq ($(PLATFORM), ti816x-evm)
ifeq ($(DDR_MEM), )
ifeq ($(DVR_RDK_BOARD_TYPE),DM816X_TI_EVM)
  	DDR_MEM := DDR_MEM_1024M
ifeq ($(LINUX_MEM),)
	LINUX_MEM := LINUX_MEM_256M
#    LINUX_MEM := LINUX_MEM_128M
endif
else
#  DDR_MEM := DDR_MEM_1024M
	DDR_MEM := DDR_MEM_2048M
ifeq ($(LINUX_MEM),)
	LINUX_MEM := LINUX_MEM_256M
#    LINUX_MEM := LINUX_MEM_128M
endif
endif
endif
  VS_CARD := WITH_VS_CARD
#  VS_CARD := WITHOUT_VS_CARD
endif

ifeq ($(PLATFORM), ti814x-evm)
ifeq ($(DDR_MEM), )
  DDR_MEM := DDR_MEM_512M
endif
ifeq ($(LINUX_MEM),)
  LINUX_MEM := LINUX_MEM_128M
endif
  VS_CARD := WITH_VS_CARD
#  VS_CARD := WITHOUT_VS_CARD
endif

##########################################################################
#Set String Format when use LINUX_MEM and DDR_MEM as suffix of filenames.
##########################################################################
DDR_SIZE_PREFIX :=
LINUX_SIZE_PREFIX :=
#Will be used as filename suffix.
DDR_SUFFIX := $(subst DDR_MEM_,$(DDR_SIZE_PREFIX),$(DDR_MEM))
PROFILE_c6xdsp := debug

USE_SYSLINK_NOTIFY=0

XDCPATH = $(hdvpss_PATH)/packages;$(bios_PATH)/packages;$(xdc_PATH)/packages;$(ipc_PATH)/packages;$(fc_PATH)/packages;$(dvr_rdk_PATH);$(syslink_PATH)/packages;$(xdais_PATH)/packages;$(edma3lld_PATH)/packages;

RPE_BUILD_VARS = ipc_PATH="${ipc_PATH}" \
	bios_PATH="${bios_PATH}" \
	xdc_PATH="${xdc_PATH}" \
	xdais_PATH="${xdais_PATH}" \
	syslink_PATH="${syslink_PATH}" \
	kernel_PATH="${KERNELDIR}" \
	CODEGEN_PATH_A8="${CODEGEN_PATH_A8}" \
	CGT_ARM_PREFIX="${CSTOOL_PREFIX}" \
	CODEGEN_PATH_DSP="${CODEGEN_PATH_DSP}" \
	CODEGEN_PATH_DSPELF="${CODEGEN_PATH_DSP}" \
	ROOTDIR="${rpe_PATH}" \
	aaclcdec_PATH="${aaclcdec_PATH}" \
	aaclcenc_PATH="${aaclcenc_PATH}"

include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(ROOTDIR)/makerules/platform.mk
include $(dvr_rdk_PATH)/component.mk

LINUX_SUFFIX :=$(shell cat ${CONFIG_BLD_XDC_m3}|sed -n 's/.*LINUX_SIZE\s*=\s*\([0-9]*\)\s*\*\s*MB/\1M/p')
LINUX_SUFFIX := $(subst ;,$(LINUX_SIZE_PREFIX),$(LINUX_SUFFIX))

export OS
export PLATFORM
export PROFILE_c6xdsp
export CODEGEN_PREFIX
export CODEGEN_PATH_A8
export CODEGEN_PATH_DSP
export bios_PATH
export xdc_PATH
export hdvpss_PATH
export dvr_rdk_PATH
export ipc_PATH
export fc_PATH
export xdais_PATH
export h264dec_DIR
export h264enc_DIR
export jpegdec_DIR
export mpeg4dec_DIR
export jpegenc_DIR
export mpeg2dec_DIR
export h264dec_PATH
export h264enc_PATH
export jpegdec_PATH
export mpeg4dec_PATH
export mpeg4enc_PATH
export mpeg4enc_DIR
export jpegenc_PATH
export mpeg2dec_PATH
export hdvicplib_PATH
export linuxdevkit_PATH
export edma3lld_PATH
export ROOTDIR
export XDCPATH
export syslink_PATH
export KERNELDIR
export DVR_RDK_BOARD_TYPE
export USE_SYSLINK_NOTIFY
export dvr_rdk_BASE
export DDR_MEM
export memcpy_neon_PATH
export rpe_PATH
export RPE_BUILD_VARS
export aaclcdec_PATH
export aaclcenc_PATH
export DDR_SUFFIX
export LINUX_SUFFIX
export TI_SW_ROOT
export watermark_lib_PATH
