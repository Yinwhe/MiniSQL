export
MAKE := make
MODE := debug
#MODE := release

BUILD = ../Build

GXX = g++
LD = ld

CFLAGS = -std=c++17
CFLAGS += -I ../API -I ./API
CFLAGS += -I ../BufferManager -I ./BufferManager
CFLAGS += -I ../CatalogManager -I ./CatalogManager
CFLAGS += -I ../IndexManager -I ./IndexManager
CFLAGS += -I ../Interpreter -I ./Interpreter
CFLAGS += -I ../RecordManager -I ./RecordManager

ifeq ($(MODE), debug)
CLFLAGS += -DDEBUG -g
endif

all:
	-@mkdir ./Build
	$(MAKE) -C ./API all
	$(MAKE) -C ./BufferManager all
	$(MAKE) -C ./CatalogManager all
	$(MAKE) -C ./IndexManager all
	$(MAKE) -C ./Interpreter all
	$(MAKE) -C ./RecordManager all
	$(GXX) $(CFLAGS) -c main.cpp -o ./Build/main.o
	$(GXX) -o MiniSQL ./Build/*.o

run:
	make all
	./MiniSQL

clean:
	-@rm -rf ./Build
	-@rm ./MiniSQL
	-@rm ./*.data
	-@rm ./*.minisql
	@echo "Clean Done!"