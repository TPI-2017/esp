CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls -DICACHE_FLASH -Wall -Wno-missing-braces -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lc -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

node-0x00000.bin: node
	esptool.py elf2image $^

node: controller.o wifi.o
	$(CC) controller.o wifi.o $(LDLIBS) $(LDFLAGS) -o node

controller.o: controller.c wifi.h

wifi.o: wifi.c wifi.h controller.h

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -vf node-0x00000.bin node-0x10000.bin *.o node
