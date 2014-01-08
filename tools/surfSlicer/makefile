############################################################################
# Makefile for the surfSlicer
# Oliver Hinds <oph@bu.edu> 2004-08-12
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

# project name
PROJECT = surfSlicer

# operating system, mac, linux and win are supported
OS = linux

# whether to compile with debug, optimize flags
DEBUG = 0
PROF = 0
OPTIM = 1

################################ APPS ################################

RM = /bin/rm -v
ECHO = /bin/echo
CC = /usr/bin/g++

#OBJDIR = ../obj
OBJDIR = .
#BINDIR = ../../bin
BINDIR = .

# location of the X includes and libraries
VTK_INCS = -I/usr/local/include/vtk
VTK_LIBS = -L/usr/local/lib/vtk -lvtkRendering -lvtkCommon -lvtkIO -lvtkGraphics

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
	OPTIM_FLAG = -O3 -s
endif

# flags for the compiler and linker
CFLAGS = -Wall -Wno-deprecated $(VTK_INCS) $(PROF_FLAG) $(DEBUG_FLAG) $(OPTIM_FLAG)
LDFLAGS = $(PROF_FLAG) -lm $(VTK_LIBS) 

############################## SUFFIXES ##############################

## if any command to generate target fails, the target is deleted.
# (see http://www.gnu.org/manual/make/html_chapter/make_5.html#SEC48)
.DELETE_ON_ERROR:

#.SUFFIXES:
#.SUFFIXES:  .o .cpp

## files that are included
GLOBAL_FILES =
EXTRA_OBJ = 

## suffix rule for subsidiary source files
# (see http://www.gnu.org/manual/make/html_chapter/make_10.html#SEC111)
%.o: %.cpp %.h 
	@$(ECHO) '[make: building $@]'
	$(CC) $(CFLAGS) -c $< 
#	mv $@ $(OBJDIR)

HDR_FILES = $(wildcard *.h)
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o) 

############################## TARGETS ###############################

default: $(PROJECT)
all:     $(PROJECT)

$(PROJECT): $(OBJ_FILES)
	@$(ECHO) 'make: building $@ for $(OS)...'
	$(CC) $(CFLAGS) $(OBJ_FILES) $(EXTRA_OBJ) -o $(PROJECT) $(LDFLAGS) 
#	mv $(PROJECT) $(BINDIR)
	@$(ECHO) '############################################'
	@$(ECHO) 'make: built [$@] successfully!'
	@$(ECHO) '############################################'

############################### CLEAN ################################

clean:
	@$(ECHO) 'make: removing object and autosave files'
	-$(RM) -f *.o *~

cleanall: clean
	@$(ECHO) 'make: removing $(PROJECT) executable'
	-/bin/rm -f $(PROJECT)

######################################################################
### $Source: /home/cvs/PROJECTS/SurfaceConstructor/tools/surfSlicer/makefile,v $
### Local Variables:
### mode: makefile
### fill-column: 76
### comment-column: 0
### End: 
