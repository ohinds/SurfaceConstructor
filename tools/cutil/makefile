############################################################################
# Makefile for the CVCNS Surface Constructor tools cutils
# Oliver Hinds <oph@bu.edu> 2004-02-04
# 
# This application is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# This software is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this application; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#######################################################################

################################ VARS ################################

# project name
export PROJECT = all

# whether to compile with debug, optimize flags
export DEBUG = 0
export OPTIM = 1
export STRIP = 1
export PROF = 0
export MEMLEAK = 0

export OS = linux

# directories 
export BIN_DIR = $(PWD)/bin

################################ APPS ################################

export RM = /bin/rm -v
export ECHO = /bin/echo
export CC = /usr/bin/gcc

# debug flag
ifeq ($(DEBUG),1)
	DEBUG_FLAG = -g
endif

# profile flag
ifeq ($(PROF),1)
	PROF_FLAG = -pg
endif

# optimize flag
ifeq ($(OPTIM),1)
	OPTIM_FLAG = -O
endif

# strip flag
ifeq ($(STRIP),1)
	STRIP_FLAG = -s
endif

# memleak catch flag
ifeq ($(MEMLEAK),1)
	MEMLEAK_FLAG = -DMEMLEAK
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
export CFLAGS = -Werror -Wall $(MEMLEAK_FLAG) $(PROF_FLAG) $(DEBUG_FLAG) $(OPTIM_FLAG) $(STRIP_FLAG) 
export LDFLAGS = $(PROF_FLAG) $(LIBSR)  $(LIBVP) -lm -lgsl -lgslcblas -lz -lglut

############################## TARGETS ###############################
all: reconFromSlices

debug: 
	$(MAKE) DEBUG=1 OPTIM=0 STRIP=0 $(PROJECT)

reconFromSlices: 
	gcc -c $(CFLAGS) $@.c
	gcc  -o $(BIN_DIR)/$@ $@.o $(LDFLAGS) $(CFLAGS) 

memtest:
	$(MAKE) DEBUG=1 OPTIM=0 STRIP=0 MEMLEAK=1 testmem

testmem: 
	gcc -DNOMAIN $(CFLAGS) -ljpeg -lm -lgsl -lgslcblas -lz -lglut $@.c -o $(BIN_DIR)/$@ /home/oph/projects/surfCon/src/*.c /home/oph/projects/libsr/obj/*.o /home/oph/projects/libvp/obj/*.o

############################### CLEAN ################################

clean:
	@$(ECHO) 'make: removing object and autosave files'
	cd $(SRC_DIR) && $(MAKE) clean
	-cd $(OBJ_DIR) && $(RM) -f *.o *~

######################################################################
### $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/cutil/makefile,v $
### Local Variables:
### mode: makefile
### fill-column: 76
### comment-column: 0
### End: 
