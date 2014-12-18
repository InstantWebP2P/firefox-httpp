
BINARIES = \
    ../../node-httpp/joyent-node-950a048/deps/UDT/src/libudt_s.a \
    src/udptransport.so \
    $(NULL)

all: build package

package:
	mkdir -p packaging/components/
	cp src/udptransport.so packaging/components/udptransport.so
	cp ../../node-httpp/joyent-node-950a048/deps/UDT/src/libudt.so packaging/components/
	cd packaging && zip -r ../udptransport.xpi ./*

build:
	make -C src

clean:
	rm -rf packaging/components/* src/*.o udptransport.xpi
