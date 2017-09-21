CC = xtensa-lx106-elf-gcc
CCP = xtensa-lx106-elf-c++
CFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall -Wno-missing-braces -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -lssl -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: tls_server.o controller.o wifi.o
	$(CC) tls_server.o controller.o wifi.o $(LDLIBS) $(LDFLAGS) -o node

controller.o: controller.c wifi.h

wifi.o: wifi.c wifi.h controller.h

tls_server.o: tls_server.c tls_server.h cert.h private_key.h

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
