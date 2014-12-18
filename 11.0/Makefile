# Please use absolute path, instead of relative path
export TOP_DIR := $(shell pwd)

### UDT path
export UDT_PATH  := $(TOP_DIR)/../../udt-iwebpp/UDT4/src/

### cross toolchain prefix
export CROSS     := 

BINARIES	= \
    ../src/httpp.so \
    $(NULL)

all: build package

package:
	mkdir -p packaging/components/
	cp -rf src/httpp.so packaging/components/httpp.so
	cd packaging && zip -r ../httpp.xpi ./*

build:
	make -C src -f Makefile

clean:
	make -C src -f Makefile clean
	rm -rf packaging/components/*.so src/*.o httpp.xpi

