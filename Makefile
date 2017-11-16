INCLUDE = /opt/espressif/sdk/include
LIBS = /opt/espressif/sdk/lib
CXX = xtensa-lx106-elf-g++
CXXFLAGS = -I$(INCLUDE) -mlongcalls -DICACHE_FLASH -DESP -Wall -std=c++11 -Wno-missing-braces -fno-strict-aliasing -Os
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lmbedtls -lc -Wl,--end-group -lgcc
LDFLAGS = -L$(LIBS) -Teagle.app.v6.ld


node-0x00000.bin: node
	esptool.py elf2image $^

node: main.o server.o message_handler.o wifi_manager.o settings.o protocolo/Message.o
	$(CXX) main.o server.o message_handler.o wifi_manager.o settings.o protocolo/Message.o $(LDLIBS) $(LDFLAGS) -o node

main.o: main.cpp wifi_manager.h settings.h server.h

message_handler.o: message_handler.cpp message_handler.h settings.h strings.h server.h protocolo/Message.h

server.o: server.cpp server.h cert.h private_key.h buffer.h message_handler.h

settings.o: settings.cpp settings.h strings.h protocolo/Message.h

wifi_manager.o: wifi_manager.cpp wifi_manager.h settings.h protocolo/Message.h strings.h

protocolo/Message.o: protocolo/Message.cpp protocolo/Message.h strings.h

flash: node-0x00000.bin
	esptool.py write_flash 0 node-0x00000.bin 0x10000 node-0x10000.bin

clean:
	rm -rvf node-0x00000.bin node-0x10000.bin *.o node
