
# Target binary name.
TARGET_PREFIX = log

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
LD_OPTS   = -lpthread -lm 
DEFINE    = $(DVR_RDK_CFLAGS)

# All the source files.
FILES = $(subst ./, , $(foreach dir,.,$(wildcard $(dir)/*.c)) )

# Headers and Libs.
PROJECT_INC  = $(PROJECT_DIR)/host/include 
INCLUDE_HEAD += -I$(PROJECT_INC)
 

SYSLINK_LIBS = $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_release
AUDIO_LIBS   = $(linuxdevkit_PATH)/usr/lib/libasound.so.2
RPE_LIBS     = $(rpe_PATH)/lib/lib/a8/debug/rpe.av5T
MCFW_LIBS    = $(MCFW_LIB_DIR)/dvr_rdk_mcfw_api.a $(MCFW_LIB_DIR)/dvr_rdk_link_api.a $(MCFW_LIB_DIR)/dvr_rdk_common_utils.a $(MCFW_LIB_DIR)/dvr_rdk_osa.a $(SYSLINK_LIBS)

LIBS = $(LIB_DIR)/$(TARGET_PREFIX).a $(MCFW_LIBS) $(AUDIO_LIBS) $(RPE_LIBS)
OBJS = $(subst .c,.o, $(FILES) )

# Search for object files.
vpath %.o $(OBJ_DIR)

LIB = $(TARGET_PREFIX).a
EXE = $(EXE_DIR)/$(TARGET_PREFIX).out

# Install the binary while do make.
all: depend lib

.c.o:
	@echo \# $(TARGET_PREFIX): $(PLATFORM): Compiling $<
	$(CC) $(CC_OPTS) $(OPTI_OPTS) $(DEFINE) $(INCLUDE_HEAD) -o$(OBJ_DIR)/$@ $<

lib : $(LIB_DIR)/$(LIB)

$(LIB_DIR)/$(LIB) : $(OBJS)
	@echo \# $(TARGET_PREFIX): $(PLATFORM): Creating archive $(LIB)
	$(AR) $(AR_OPTS) $(LIB_DIR)/$(LIB) $(OBJ_DIR)/*.o

obj: $(OBJS)

clean:
	rm -rf $(BUILD_DIR)

depend:
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(EXE_DIR)
	-mkdir -p $(LIB_DIR)

.PHONY: all clean install

