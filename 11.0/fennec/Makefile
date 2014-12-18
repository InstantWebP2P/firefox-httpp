# Please use absolute path, instead of relative path

### mozilla, android sdk path
export MOZCONFIG := /ISS/IWEBPP/mozilla-src/mozconfig-droid
export MOZSDKPATH:= /ISS/IWEBPP/mozilla-src/objdir-droid/dist/sdk/

export ANDROIDNDK:= /ISS/android/android-ndk-r5c/
export ANDROIDSDK:= /ISS/android/android-sdk-linux_x86/

### UDT path
export UDT_PATH  := /ISS/IWEBPP/iwebpp/trunk/node-httpp/joyent-node-950a048/deps/UDT/src/

### android platforms version and API version
export ANDROIDVER:=5
export ANDROIDAPI:=13

### cross toolchain prefix
export CROSS     := $(ANDROIDNDK)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-

BINARIES	= \
    ../src/httpp.so \
    $(NULL)

all: build package

package:
	mkdir -p packaging/components/
	cp -rf ../src/httpp.so packaging/components/httpp.so
	cd packaging && zip -r ../httpp-fennec.xpi ./*

build:
	make -C ../src -f Makefile.fennec

clean:
	make -C ../src -f Makefile.fennec clean
	rm -rf packaging/components/*.so ../src/*.o httpp-fennec.xpi

