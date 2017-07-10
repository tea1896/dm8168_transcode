
# Target binary name.
TARGET_PREFIX = dm8168_transcode

# Install the target binary.
INSTALL_DIR=$(INSTALL_PATH)/bin

CC = $(CODEGEN_PREFIX)gcc
AR = $(CODEGEN_PREFIX)ar
LD = $(CODEGEN_PREFIX)gcc

BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
EXE_DIR   = $(BUILD_DIR)/bin
LIB_DIR   = $(BUILD_DIR)/lib

MCFW_LIB_DIR = $(dvr_rdk_PATH)/lib/$(PLATFORM)

# Search for library.
vpath %.a $(MCFW_LIB_DIR)

CC_OPTS   = -c -Wall -Warray-bounds -Werror
OPTI_OPTS = -O3
AR_OPTS   = -rc
LD_OPTS   = -lpthread -lm -lavformat -lavcodec -lavdevice -lavutil -lavfilter -lpostproc -lswresample -lswscale -lxvidcore -lyasm -L/home/ti/8168/third-party-lib/ffmpeg/build/lib
DEFINE    = $(DVR_RDK_CFLAGS)

# All the source files.
FILES = $(subst ./, , $(foreach dir,.,$(wildcard $(dir)/*.c)) )

# Headers and Libs.
OSA_INC          = -I$(dvr_rdk_PATH)/mcfw/src_linux/osa/inc
DEVICES_INC      = -I$(dvr_rdk_PATH)/mcfw/src_linux/devices/inc -I$(dvr_rdk_PATH)/mcfw/src_linux/devices/tvp5158/inc -I$(dvr_rdk_PATH)/mcfw/src_linux/devices/sii9022a/inc  -I$(dvr_rdk_PATH)/mcfw/src_linux/devices/thsfilters/inc -I$(dvr_rdk_PATH)/mcfw/src_linux/devices/tw2968/inc
OSA_KERMOD_INC   = -I$(dvr_rdk_PATH)/mcfw/src_linux/osa_kermod/inc
COMMON_UTILS_INC = -I$(dvr_rdk_PATH)/mcfw/src_common/utils/inc
SYSLINK_INC      = -I$(syslink_PATH)/packages -I$(ipc_PATH)/packages
CODEC_INC        = -I$(xdais_PATH)/packages -I$(h264dec_PATH)/Inc -I$(mpeg4dec_PATH)/Inc -I$(mpeg2dec_PATH)/inc -I$(h264enc_PATH)/Inc -I$(jpegdec_PATH)/Inc -I$(jpegenc_PATH)/Inc -I$(mpeg4enc_PATH)/Inc
LINUXDEVKIT_INC  = -I$(linuxdevkit_PATH)/usr/include
RPE_INC          = -I$(rpe_PATH)/include -I$(rpe_PATH)/src/include -I$(aaclcdec_PATH)/packages -I$(aaclcenc_PATH)/packages -I$(xdais_PATH)/packages
MCFW_INC         = -I$(dvr_rdk_PATH)/mcfw/interfaces -I$(dvr_rdk_PATH)/mcfw/interfaces/common_def
COMMON_INC       = -I../inc -I../priv -I. -I$(dvr_rdk_PATH)

INCLUDE += $(MCFW_INC) $(OSA_INC) $(DEVICES_INC) $(RPE_INC) $(LINUXDEVKIT_INC) $(SYSLINK_INC) \
		   $(CODEC_INC) $(KERNEL_INC) $(COMMON_INC)
INCLUDE += -I$(KERNELDIR)/usr/include -I$(KERNELDIR)/arch/arm/include/  

SYSLINK_LIBS = $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_release
AUDIO_LIBS   = $(linuxdevkit_PATH)/usr/lib/libasound.so.2
RPE_LIBS     = $(rpe_PATH)/lib/lib/a8/debug/rpe.av5T
MCFW_LIBS    = $(MCFW_LIB_DIR)/dvr_rdk_mcfw_api.a $(MCFW_LIB_DIR)/dvr_rdk_link_api.a $(MCFW_LIB_DIR)/dvr_rdk_common_utils.a $(MCFW_LIB_DIR)/dvr_rdk_osa.a $(SYSLINK_LIBS)

LIBS = $(LIB_DIR)/$(TARGET_PREFIX).a $(MCFW_LIBS) $(AUDIO_LIBS) $(RPE_LIBS)

LIBS+=rtsp_receiver/libRTSPReceiver.a -lstdc++
LIBS+=$(THIRD_PARTY_LIB_PATH)/live/liveMedia/libliveMedia.a 
LIBS+=$(THIRD_PARTY_LIB_PATH)/live/groupsock/libgroupsock.a 
LIBS+=$(THIRD_PARTY_LIB_PATH)/live/BasicUsageEnvironment/libBasicUsageEnvironment.a 
LIBS+=$(THIRD_PARTY_LIB_PATH)/live/UsageEnvironment/libUsageEnvironment.a

LIBS+=mpegts/libMpegTS.a

OBJS = $(subst .c,.o, $(FILES) )

# Search for object files.
vpath %.o $(OBJ_DIR)

LIB = $(TARGET_PREFIX).a
EXE = $(EXE_DIR)/$(TARGET_PREFIX).out

# Install the binary while do make.
all: depend mpegtslib rtsp lib exe install

.c.o:
	@echo \# $(TARGET_PREFIX): $(PLATFORM): Compiling $<
	$(CC) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE) -o$(OBJ_DIR)/$@ $<

lib : $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB) : $(OBJS)
	@echo \# $(TARGET_PREFIX): $(PLATFORM): Creating archive $(LIB)
	$(AR) $(AR_OPTS) $(LIB_DIR)/$(LIB) $(OBJ_DIR)/*.o

obj: $(OBJS)

clean:
	rm -rf $(BUILD_DIR)
	make -C rtsp_receiver clean
	make -C mpegts clean

depend:
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(EXE_DIR)
	-mkdir -p $(LIB_DIR)

exe:
	@echo \# $(TARGET_PREFIX): $(PLATFORM): Linking
	$(LD) $(LD_OPTS) -o$(EXE) $(LIBS)
	@echo \# Final executable $(EXE) !!!
	@echo \#

rtsp:
	make -C rtsp_receiver
	
mpegtslib:
	make -C mpegts

install:
	cp $(EXE) $(INSTALL_DIR)

.PHONY: all clean install

