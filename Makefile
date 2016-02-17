#####################################################################
#
#  Name:         Makefile
#  Created by:   Stefan Ritt
#
#  Contents:     Makefile for MIDAS example frontend and analyzer
#
#  $Id$
#
#####################################################################
#
#--------------------------------------------------------------------
# The MIDASSYS should be defined prior the use of this Makefile
ifndef MIDASSYS
missmidas::
	@echo "...";
	@echo "Missing definition of environment variable 'MIDASSYS' !";
	@echo "...";
endif

ARCH := $(shell uname)
ifeq ($(ARCH),Linux)
OS_DIR = linux
OSFLAGS = -DOS_LINUX -Dextname
#CFLAGS = -Wall -lrt
CFLAGS = -Wno-cpp -lrt
# add to compile in 32-bit mode
# OSFLAGS += -m32
LIBS = -lm -lz -lutil -lnsl -lpthread -lrt #-lgsl -lgslcblas #-lvme
endif

ifeq ($(ARCH),Darwin)
OS_DIR = darwin
OSFLAGS = -DOS_LINUX -Dextname
CFLAGS = -Wno-cpp -lrt
LIBS = -lm -lz -lutil
endif

#-----------------------------------------
# ROOT flags and libs
#
ifdef ROOTSYS
ROOTCFLAGS := $(shell  $(ROOTSYS)/bin/root-config --cflags)
ROOTCFLAGS += -DHAVE_ROOT -DUSE_ROOT
ROOTLIBS   := $(shell  $(ROOTSYS)/bin/root-config --libs) -Wl,-rpath,$(ROOTSYS)/lib
ROOTLIBS   += -lThread
#SiliconDataDict.cxx:	SiliconDataDict.h
#	rootcint -f $@ -c $^
#SiliconDataDict.o: SiliconDataDict.cxx SiliconDataDict.h
else
missroot:
	@echo "...";
	@echo "Missing definition of environment variable 'ROOTSYS' !";
	@echo "...";
endif
#-------------------------------------------------------------------
# The following lines define directories. Adjust if necessary
#                 
DRV_DIR   = $(MIDASSYS)/drivers/vme/vmic
VMEDRV_DIR   = $(MIDASSYS)/drivers/vme
# this is for vme modules driver directory.
MODULES_DIR= $(MIDASSYS)/drivers/vme
INC_DIR   = $(MIDASSYS)/include
LIB_DIR   = $(MIDASSYS)/$(OS_DIR)/lib
# LIB_DIR = $(MIDASSYS)/lib
SRC_DIR   = $(MIDASSYS)/src

#-------------------------------------------------------------------
# List of analyzer modules
#
MODULES   = adc.o qdc.o scaler.o SiliconData.o GammaData.o Parameters.o multiTDC.o MMM.o W1.o RawData.o HagarSort.o FocalPlane.o main.o

#-------------------------------------------------------------------
# Hardware driver can be (camacnul, kcs2926, kcs2927, hyt1331)
#
DRIVER = vmicvme
VMEMODULES = v1190A v259 v792 v830
VMEMODULES_OBJ = $(addsuffix .o, $(VMEMODULES))
#-------------------------------------------------------------------
# Frontend code name defaulted to frontend in this example.
# comment out the line and run your own frontend as follow:
# gmake UFE=my_frontend
#
UFE = k600frontend

####################################################################
# Lines below here should not be edited
####################################################################

# MIDAS library
LIB = $(LIB_DIR)/libmidas.a

# compiler
CC = cc
CXX = g++
CFLAGS += -g -I$(INC_DIR) -I$(DRV_DIR) -I$(MODULES_DIR) -I$(VMEDRV_DIR)
LDFLAGS += -L $(LIB)

#all: $(UFE) analyzer 
all: analyzer 

$(UFE): $(LIB_DIR)/mfe.o $(DRIVER).o $(VMEMODULES_OBJ) $(UFE).o  
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

$(UFE).o: $(UFE).c 
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(UFE).o $(UFE).c

$(DRIVER).o: $(DRV_DIR)/$(DRIVER).c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $(DRIVER).o $(DRV_DIR)/$(DRIVER).c

%.o: $(VMEDRV_DIR)/%.c
	$(CC) $(CFLAGS) $(OSFLAGS) -c -o $@ $<

$(MODULE_DIR)/%.o: $(MODULES_DIR)/%.c
	$(CXX) $(USERFLAGS) $(ROOTCFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

analyzer: $(LIB) $(LIB_DIR)/rmana.o analyzer.o $(MODULES)
	$(CXX) $(CFLAGS) -o $@ $(LIB_DIR)/rmana.o analyzer.o $(MODULES) \
	$(LIB) $(LDFLAGS) $(ROOTLIBS) $(LIBS) 

%.o: %.c experim.h
	$(CXX) $(USERFLAGS) $(ROOTCFLAGS) $(CFLAGS) $(OSFLAGS) -o $@ -c $<

clean::
	rm -f *.o *~ \#*

#end file
