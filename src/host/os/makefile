TARGET = wv_common.a

# ARCH= armv5l-Linux-
CC= $(ARCH)gcc
LD = $(ARCH)ld
AS = $(ARCH)as
CPP = $(ARCH)g++
AR = $(ARCH)ar
CFLAGS= -g -O2 -D_GNU_SOURCE -DHAVE_CONFIG_H -I

BUILD_DIR = build
OBJ_DIR   = $(BUILD_DIR)/obj
EXE_DIR   = $(BUILD_DIR)/bin
LIB_DIR   = $(BUILD_DIR)/lib

LIBS= -lpthread -lrt -lm
INCLUDE= -I.  -I/home/ti/8168/samba/dm8168_transcode/src/host/include  -I/home/ti/8168/samba/dm8168_transcode/src/host/comm/Log

APPDIR= $(PWD)
APPINCH=$(shell find $(APPDIR) -name "*.h*")
TMPDIR=$(dir $(APPINCH))
APPINCDIR=$(sort $(TMPDIR))
INCLUDE += $(foreach temp, $(APPINCDIR), -I$(temp))
SRC=$(shell find $(APPDIR) -name '*.c')
APP=$(SRC:%.c=%.o)

all: depend   $(TARGET) 

$(APP): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	mv $@ $(OBJ_DIR)

$(TARGET) : clean $(APP)
	$(ARCH)ar -crs $@ $(OBJ_DIR)/*.o 
	# $(CC) -o$@ $(LIBS) $(INCLUDE) $(APP)
	
depend:
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(EXE_DIR)
	-mkdir -p $(LIB_DIR)

clean:
	-rm -rf $(APP)
	-rm -f main

.PHONY: clean
