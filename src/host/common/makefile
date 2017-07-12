PROJECT_DIR = /home/ti/8168/samba/dm8168_transcode/src
PROJECT_INC	= -I$(PROJECT_DIR)/host/include

TARGET = wv_common.a
CUR_DIR = /home/ti/8168/samba/dm8168_transcode/src/host/common

BUILD_DIR = build
OBJ_DIR   = $(CUR_DIR)/$(BUILD_DIR)/obj
EXE_DIR   = $(CUR_DIR)/$(BUILD_DIR)/bin
LIB_DIR   = $(CUR_DIR)/$(BUILD_DIR)/lib

#ARCH= armv5l-Linux-
CC= $(CODEGEN_PREFIX)gcc
LD = $(CODEGEN_PREFIX)ld
AS = $(CODEGEN_PREFIX)as
CPP = $(CODEGEN_PREFIX)g++
AR = $(CODEGEN_PREFIX)ar
CFLAGS= -g -O2 -D_GNU_SOURCE -DHAVE_CONFIG_H

LIBS= -lpthread -lrt -lm
INCLUDE= -I.

# APPDIR= $(PWD)
APPDIR = /home/ti/8168/samba/dm8168_transcode/src/host/common
APPINCH=$(shell find $(APPDIR) -name "*.h*")
TMPDIR=$(dir $(APPINCH))
APPINCDIR=$(sort $(TMPDIR))
INCLUDE += $(foreach temp, $(APPINCDIR), -I$(temp))
INCLUDE += $(PROJECT_INC)
SRC=$(shell find $(APPDIR) -name '*.c')
APP=$(SRC:%.c=%.o)

all:  depend  $(TARGET) 

$(APP): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	mv  $@ $(OBJ_DIR)

$(TARGET): $(APP)
	$(ARCH)ar -crs $(LIB_DIR)/$@  $(OBJ_DIR)/*.o
		
depend:
	-mkdir -p $(OBJ_DIR)
	-mkdir -p $(EXE_DIR)
	-mkdir -p $(LIB_DIR)

clean:
	-rm -rf $(APP)
	-rm -f $(TARGET)
	rm -rf $(BUILD_DIR)

.PHONY: clean
