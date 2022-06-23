#************************************************
#*                                              *
#*             (c) 2017 J. FABRIZIO             *
#*                                              *
#*                               LRDE EPITA     *
#*                                              *
#************************************************

CC = g++

CPP_FILES = src/matrix.cc src/program.cc src/input.cc src/lib/obj.cc src/utils.cc src/shader_func.cc src/textures.cc src/lib/stb_image.cc
CPP_FILES +=
HXX_FILES = src/matrix.hh src/program.hh src/object_vbo.hh src/input.hh src/camera.hh src/mouse.hh src/lib/obj.hh src/utils.hh src/shader_func.hh src/textures.hh src/lib/stb_image.h
HXX_FILES +=
OBJ_FILES = $(CPP_FILES:.cc=.o)

CXX_FLAGS += -Wall -Wextra -O3 -g -std=c++17
CXX_FLAGS +=
CXX_FLAGS += -m64 -march=native
CXX_FLAGS += -fopt-info-vec-optimized #-fopt-info-vec-missed -ftree-vectorize
LDXX_FLAGS = -lGLEW -lglfw -lGL -lX11 -lXrandr -lrt -lm
# LDXX_FLAGS += -lXi -lXxf86vm -lXinerama -lXcursor 

MAIN_FILE = src/main.cc
DIST = main

SKEL_FILES = $(CPP_FILES) $(HXX_FILES) $(MAIN_FILE)


#For gcc 4.9
#CXXFLAGS+=-fdiagnostics-color=auto
export GCC_COLORS=1

define color
    if test -n "${TERM}" ; then\
	if test `tput colors` -gt 0 ; then \
	    tput setaf $(1); \
        fi;\
    fi
endef

define default_color
    if test -n "${TERM}" ; then\
	if test `tput colors` -gt 0 ; then  tput sgr0 ; fi; \
    fi
endef


all: post-build

pre-build:
	@$(call color,4)
	@echo "******** Starting Compilation ************"
	@$(call default_color)

post-build:
	@make --no-print-directory main-build ; \
	sta=$$?;	  \
	$(call color,4); \
	echo "*********** End Compilation **************"; \
	$(call default_color); \
	exit $$sta;

main-build: pre-build build

build: $(OBJ_FILES)
	$(CC) $(MAIN_FILE) -o $(DIST) $(OBJ_FILES) $(CXX_FLAGS) $(LDXX_FLAGS)

run: pre-build build
	./${DIST}

%.o: %.cc %.hh
	@$(call color,2)
	@echo "[$@] $(CXX_FLAGS)"
	@$(call default_color)
	@$(CC) -c -o $@ $< $(CXX_FLAGS) ; \
	sta=$$?;	  \
	if [ $$sta -eq 0 ]; then  \
	  $(call color,2) ; \
	  echo "[$@ succes]" ; \
	  $(call default_color) ; \
	else  \
	  $(call color,1) ; \
	  echo "[$@ failure]" ; \
	  $(call default_color) ; \
	fi ;\
	exit $$sta

.PHONY: all clean pre-build post-build main-build build run

clean:
	rm -f $(OBJ_FILES)
	rm -f $(DIST)