

INCLUDES = -I$(FFMPEG_LIB_PATH)/include  \
           -I$(FFMPEG_LIB_PATH)/include/libavformat \
		   -I$(PROJECT_DIR)/host/include
		   
		 
##### Change the following for your environment: 
CROSS_COMPILE?=		$(CODEGEN_PREFIX)
COMPILE_OPTS =		$(INCLUDES) -I. -O3 -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -lavformat -lavcodec -lavdevicde -lavutil -lpostproc -lswresamole -lswscale -lavfilter -L$(FFMPEG_LIB_PATH)/lib
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
AR =			$(CROSS_COMPILE)ar
LIB_SUFFIX =			a
##### End of variables to change

TARGET = libMpegTS.a

all: $(TARGET) 

RTSP_RECEIVER_OBJS = tsInput.o

$(TARGET):$(RTSP_RECEIVER_OBJS) $(LOCAL_LIBS)
	$(AR) -crs $@ *.o 

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

	
# FFMPEG_LIB_DIR = $(FFMPEG_LIB_PATH)/lib

# LOCAL_LIBS = $(FFMPEG_LIB_DIR)/libavcodec.a  \
			 # $(FFMPEG_LIB_DIR)/libavfilter.a \
			 # $(FFMPEG_LIB_DIR)/libavutil.a \
			 # $(FFMPEG_LIB_DIR)/libswresample.a \
			 # $(FFMPEG_LIB_DIR)/libavdevice.a \
			 # $(FFMPEG_LIB_DIR)/libavformat.a \
			 # $(FFMPEG_LIB_DIR)/libpostproc.a \
			 # $(FFMPEG_LIB_DIR)/libswscale.a \
			 # $(FFMPEG_LIB_DIR)/libxvidcore.a \
			 # $(FFMPEG_LIB_DIR)/libx264.a \

LIBS =			$(LOCAL_LIBS) $(LIBS_FOR_CONSOLE_APPLICATION)

clean:
	-rm -rf *.$(OBJ) $(TARGET)

##### Any additional, platform-specific rules come here:
