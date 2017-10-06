CXX = xtensa-lx106-elf-g++
CXXFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall -std=c++11 -Wno-missing-braces -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -lssl -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: main.o server.o controller.o wifi_manager.o
	$(CXX) main.o server.o controller.o wifi_manager.o $(LDLIBS) $(LDFLAGS) -o node

main.o: main.cpp wifi_manager.h

controller.o: controller.cpp wifi_manager.h

wifi_manager.o: wifi_manager.cpp wifi_manager.h controller.h

server.o: server.cpp server.h cert.h private_key.h buffer.h

test.o: test.cpp

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
