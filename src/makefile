
DVR_RDK_ROOT_PATH = /home/ti/8168/sdk/tl-mcfw-rdk-1.3
export DVR_RDK_ROOT_PATH

# Install the target files
INSTALL_PATH = /home/ti/8168/rfs_816x/816x-dvr-rdk-nfs-1.0/opt/dvr_rdk/ti816x
export INSTALL_PATH

# live555
THIRD_PARTY_LIB_PATH = /home/ti/8168/third-party-lib
export THIRD_PARTY_LIB_PATH

include Rules.make

all:
	make -C host all
	#make -C dsp all

clean:
	make -C host clean
	#make -C dsp clean
