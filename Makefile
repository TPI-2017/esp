CXX = xtensa-lx106-elf-g++
CXXFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall -std=c++11 -Wno-missing-braces -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -lssl -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: tls_server.o controller.o wifi.o
	$(CXX) tls_server.o controller.o wifi.o $(LDLIBS) $(LDFLAGS) -o node

controller.o: controller.cpp wifi.h

wifi.o: wifi.cpp wifi.h controller.h

tls_server.o: tls_server.cpp tls_server.h cert.h private_key.h

test.o: test.cpp

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
