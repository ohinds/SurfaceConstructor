############################################################################
# Makefile for the CVCNS Surface Constructor
# Oliver Hinds <oph@cns.bu.edu> 2004-02-04
#######################################################################

################################ VARS ################################

# project name
export PROJECT = surfCon

# operating system, mac, linux and win are supported
export OS = linux

# whether to compile with debug, optimize flags
export DEBUG = 0
export OPTIM = 1
export STRIP = 1
export PROF = 0
export MEMLEAK = 0

# directories 
export DEST_DIR = /usr/local/bin/
export BIN_DIR = $(PWD)/bin
export SRC_DIR = $(PWD)/src
export OBJ_DIR = $(PWD)/obj

# whether image exporting should be compiled in 
# requires offscreen rendering support
export IMG_EXPORT = 0

################################ APPS ################################

export RM = /bin/rm -v
export ECHO = /bin/echo
export CC = /usr/bin/gcc
export INSTALL=/usr/bin/install

# location of the X includes and libraries
X_INCS = -I/usr/X11R6/include
X_LIBS = -L/usr/X11R6/lib -lXext -lX11 -lXmu -lXxf86vm -L/usr/lib64 -L/usr/X11R6/lib64

# library linking options
SR_LIB_TYPE = shared
VP_LIB_TYPE = shared


# jpeg includes and libs
ifeq ($(OS),mac)
	JPEG_INCS = -I$(FINK_ROOT)/sw/include
	JPEG_LIBS = -L$(FINK_ROOT)/sw/lib -ljpeg
else
	JPEG_INCS = -I/usr/include
	JPEG_LIBS = -L/usr/lib -ljpeg
endif

# location of the OpenGL includes and libraries
GL_INCS = -I/usr/include
GL_LIBS = -L/usr/lib -lglut -lGLU -lGL -lpthread

# debug flag
ifeq ($(DEBUG),1)
	DEBUG_FLAG = -g
endif

# profile flag
ifeq ($(PROF),1)
	PROF_FLAG = -pg
endif

# memleak flag
ifeq ($(MEMLEAK),1)
	MEMLEAK_FLAG = -DMEMLEAK
	VP_LIB_TYPE = static
	SR_LIB_TYPE = static
endif

# optimize flag
ifeq ($(OPTIM),1)
	OPTIM_FLAG = -O
endif

# strip flag
ifeq ($(STRIP),1)
	STRIP_FLAG = -s
endif

# params for static compilation of libs
ifeq ($(OS),win)
	SR_LIB_TYPE = static
endif
ifeq ($(OS),mac)
	SR_LIB_TYPE = static
endif
ifeq ($(SR_LIB_TYPE),static)
	LIBSR_LIB_DIR = /usr/local/lib
	LIBSR = $(LIBSR_LIB_DIR)/libsr.a
#	LIBSR = ../SurfaceReconstructionLibrary/obj/*.o
else
	LIBSR = -lsr
endif

ifeq ($(OS),win)
	VP_LIB_TYPE = static
endif
ifeq ($(OS),mac)
	VP_LIB_TYPE = static
endif
ifeq ($(VP_LIB_TYPE),static)
	LIBVP_LIB_DIR = /usr/local/lib
	LIBVP = $(LIBVP_LIB_DIR)/libvp.a
#	LIBVP = ../VolumeProcessingLibrary/obj/*.o
else
	LIBVP = -lvp
endif

# flags for the compiler and linker
export CINCL =  -I$(SRC_DIR)
export CFLAGS = -Werror -Wall $(MEMLEAK_FLAG) $(PROF_FLAG) $(JPEG_INCS) $(DEBUG_FLAG) $(OPTIM_FLAG) $(STRIP_FLAG) $(CINCL) 
export LDFLAGS = $(PROF_FLAG) $(MEMLEAK_FLAG) $(JPEG_LIBS) $(LIBSR) $(LIBVP) -lm -lgsl -lgslcblas -lz

# differences between mac and *nix
ifeq ($(OS),mac)
	CFLAGS += -DMAC
	LDFLAGS += -framework OpenGL -framework GLUT -lobjc
else
	CFLAGS +=  $(GL_INCS) $(X_INCS) 
	LDFLAGS += $(GL_LIBS) $(X_LIBS)
endif

# specific flags for windows, these override earlier defs of cflags and ldflags
ifeq ($(OS),win)
#        export CFLAGS =  $(LIBSR) $(LIBVP) -Wall -Werror -I/usr/include $(PROF_FLAG) $(JPEG_INCS) $(DEBUG_FLAG) $(OPTIM_FLAG) $(STRIP_FLAG)
	export LDFLAGS = $(LIBVP) $(LIBSR) $(PROF_FLAG) $(MEMLEAK_FLAG) -L/usr/X11R6/lib -lglut -lGLU -lGL $(JPEG_LIBS) -lm -lgsl -lgslcblas -lz  -lopengl32 -lglu32 -lglut32 
endif

# if image exporting should be compiled in
ifeq ($(IMG_EXPORT),1)
	CFLAGS += -DIMG_EXPORT
	LD_FLAGS += -lOSMesa
endif

############################### SUFFIXES ##############################

OBJ_FILES = $(wildcard $(OBJ_DIR)/*.o)

############################## TARGETS ###############################

default: $(PROJECT)
all:     $(PROJECT)
debug:	 
	$(MAKE) DEBUG=1 OPTIM=0 STRIP=0 $(PROJECT)

profile:	 
	$(MAKE) PROF=1 DEBUG=1 OPTIM=0 STRIP=0 $(PROJECT)

memleak:	 
	$(MAKE) MEMLEAK=1 DEBUG=1 OPTIM=0 STRIP=0 $(PROJECT)

$(PROJECT): $(OBJ_FILES)
	@$(ECHO) 'make: building $@ for $(OS)...'
	cd $(SRC_DIR) && $(MAKE)
	$(CC) -o $(BIN_DIR)/$(PROJECT) $(OBJ_DIR)/*.o $(CFLAGS) $(LDFLAGS) 
	@$(ECHO) '############################################'
	@$(ECHO) 'make: built [$@] successfully!'
	@$(ECHO) '############################################'

$(PROJECT)64: $(OBJ_FILES)
	@$(ECHO) 'make: building $@ for $(OS)...'
	cd $(SRC_DIR) && $(MAKE)
	$(CC) $(CFLAGS) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$(PROJECT)64 $(LDFLAGS) 
	@$(ECHO) '############################################'
	@$(ECHO) 'make: built [$@] successfully!'
	@$(ECHO) '############################################'

install: 
	cd $(SRC_DIR) && $(MAKE)
	$(CC) -o $(BIN_DIR)/$(PROJECT) $(OBJ_DIR)/*.o $(CFLAGS) $(LDFLAGS) 
	@$(ECHO) 'make: installing [$(PROJECT)] for $(OS)...'
ifeq ($(OS),linux)
	$(INSTALL) $(BIN_DIR)/$(PROJECT) $(DEST_DIR)
else
	cp $(BIN_DIR)/$(PROJECT) $(DEST_DIR)
endif
	@$(ECHO) '############################################'
	@$(ECHO) 'make: installed [$(PROJECT)] successfully!'
	@$(ECHO) '############################################'

############################### CLEAN ################################

clean:
	@$(ECHO) 'make: removing object and autosave files'
	cd $(SRC_DIR) && $(MAKE) clean
	-cd $(OBJ_DIR) && $(RM) -f *.o *~

######################################################################
### $Source: /home/cvs/PROJECTS/SurfaceConstructor/makefile,v $
### Local Variables:
### mode: makefile
### fill-column: 76
### comment-column: 0
### End: 
