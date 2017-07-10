
# Common source files and CFLAGS across all platforms and cores
SRCS_c6xdsp = main_c6xdsp.c app_link.c

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc hdvpss ipc syslink dvr_rdk fc xdais h264enc h264dec \
							 mpeg4dec mpeg2dec swosd edma3lld
# List all the components required by the application
COMP_LIST_c6xdsp  = dvr_rdk ipc syslink

CFLAGS_LOCAL_c6xdsp = $(DVR_RDK_CFLAGS)

# Include make paths and options for all supported targets/platforms
include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(ROOTDIR)/makerules/platform.mk

.PHONY : all clean gendirs m3video m3vpss c6xdsp host

all : $(CORE)

BUILD_DIR = build
OBJDIR    = $(BUILD_DIR)/obj
BINDIR    = $(BUILD_DIR)/bin

APP_NAME            = dvr_rdk
XDC_CFG_FILE_c6xdsp = MAIN_APP_c6xdsp.cfg
CONFIGURO_DIRNAME   = $(APP_NAME)_configuro
CONFIGURO_DIR       = $(DEST_ROOT)/$(APP_NAME)/obj/$(PLATFORM)/$(CORE)/$(PROFILE_$(CORE))/$(CONFIGURO_DIRNAME)
XDC_CFG_FILE_NAME   = $(XDC_CFG_FILE_$(CORE))

DEPDIR  = $(OBJDIR)/.deps
DEPFILE = $(DEPDIR)/$(*F)

# Create directories
$(OBJDIR) :
	mkdir -p $(OBJDIR)

$(BINDIR) :
	mkdir -p $(BINDIR)

$(DEPDIR) :
	mkdir -p $(DEPDIR)

# Assemble list of source file names
SRCS  = $(SRCS_COMMON) $(SRCS_$(CORE)) $(SRCS_$(SOC)) $(SRCS_$(PLATFORM))
ASRCS = $(ASRCS_$(CORE)) $(ASRCS_$(SOC)) $(ASRCS_$(PLATFORM))

# Define search paths
VPATH = .

# Derive list of all packages from each of the components needed by the app
PKG_LIST = $(foreach COMP,$(COMP_LIST_$(CORE)),$($(COMP)_PKG_LIST))

# For each of the packages (or modules), get a list of source files that are
# marked to be compiled in app stage of the build (or in the context in the app)
SRCS_APPSTG_FILES = $(foreach PKG, $(PKG_LIST), $($(PKG)_APP_STAGE_FILES))
# The app has to compile package cfg source files in its context. The name
# of the file assumed is <MOD>_cfg.c under the top-level directory - i.e.
# specified by <MOD>_PATH variable
#SRCS_CFG = $(addsuffix _cfg.c,$(PKG_LIST))
SRCS += $(SRCS_APPSTG_FILES)
PKG_PATHS = $(foreach PKG,$(PKG_LIST),$($(PKG)_PATH))
VPATH += $(PKG_PATHS)

# Change the extension from C to $(OBJEXT) and also add path
OBJ_PATHS = $(patsubst %.c, $(OBJDIR)/%.$(OBJEXT), $(SRCS))

AOBJ_PATHS = $(patsubst %.s, $(OBJDIR)/%.$(AOBJEXT), $(ASRCS))

# Assemble include paths here
INCLUDE_EXTERNAL = $(foreach INCL,$(INCLUDE_EXERNAL_INTERFACES),$($(INCL)_INCLUDE))
INCLUDE_ALL = $(CODEGEN_INCLUDE) . $(INCLUDE_EXTERNAL) 

# Add prefix "-I" to each of the include paths in INCLUDE_ALL
INCLUDES = $(addprefix -I,$(INCLUDE_ALL))

# Create rule to "make" all packages
.PHONY : $(PKG_LIST)
$(PKG_LIST) :
	echo \# Making $(PLATFORM):$(CORE):$(PROFILE_$(CORE)):$@...
	make -C $($@_PATH)

# Get libraries for all the packages
# LIBS = $(foreach LIB,$(PKG_LIST),$(LIB).$(LIBEXT))
define GET_COMP_DEPENDENCY_SUB_PATH
  ifeq ($$($(1)_PLATFORM_DEPENDENCY),yes)
    ifeq ($$($(1)_CORE_DEPENDENCY),yes)
      $(1)_DEPSUBPATH = $(PLATFORM)/$(CORE)
    else
      $(1)_DEPSUBPATH = $(PLATFORM)/c674
    endif
   else
    ifeq ($$($(1)_CORE_DEPENDENCY),yes)
      $(1)_DEPSUBPATH = $(CORE)
    else
      $(1)_DEPSUBPATH = c674
    endif
  endif
endef

$(foreach LIB,$(PKG_LIST),$(eval $(call GET_COMP_DEPENDENCY_SUB_PATH,$(LIB))))

MCFW_BUILD_IDR = $(dvr_rdk_PATH)/build
LIB_PATHS = $(foreach LIB,$(PKG_LIST),$(MCFW_BUILD_IDR)/$($(LIB)_RELPATH)/lib/$($(LIB)_DEPSUBPATH)/$(PROFILE_$(CORE))/$(LIB).$(LIBEXT))

include $(ROOTDIR)/makerules/rules_c674.mk

# Clean Object, Binary and Configuro generated directories
clean :
	rm -rf $(BUILD_DIR)

# Create dependencies list to ultimately create application executable binary
$(CORE) : $(OBJDIR) $(BINDIR) $(DEPDIR) $(PKG_LIST) $(EXE_NAME)

