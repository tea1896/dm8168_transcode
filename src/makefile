
DVR_RDK_ROOT_PATH = /home/ti/8168/sdk/tl-mcfw-rdk-1.3
export DVR_RDK_ROOT_PATH

# Install the target files
INSTALL_PATH = /home/ti/8168/rfs_816x/816x-dvr-rdk-nfs-1.0/opt/dvr_rdk/ti816x/bin/multich_transcode
export INSTALL_PATH

# live555
THIRD_PARTY_LIB_PATH = /home/ti/8168/third-party-lib
export THIRD_PARTY_LIB_PATH

# ffmpeg
FFMPEG_LIB_PATH = $(THIRD_PARTY_LIB_PATH)/ffmpeg/build
export  FFMPEG_LIB_PATH

# Project path
PROJECT_DIR = $(shell pwd)
export PROJECT_DIR

#Porject include
PROJECT_INC		 = -I$(PROJECT_DIR)/host/include  \
                   -I$(PROJECT_DIR)/host/common/Log
export PROJECT_INC

include Rules.make

all:
	make -C host all

clean:
	make -C host clean
