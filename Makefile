MCU = atxmega128a4u
FORMAT = ihex
TARGET = ecg

CC = avr-g++
CXX = avr-g++
LIBS =
INCLUDE = -Iinclude -Idrivers/include -Ifreertos/include \
 		  -Ifreertos/portable/GCC/ATxmega128A4U/
CFLAGS = -O2 -mmcu=$(MCU) $(INCLUDE) -DF_CPU=32000000
CXXFLAGS = -O2 -mmcu=$(MCU) $(INCLUDE) -DF_CPU=32000000
LD = avr-g++
LDFLAGS = -O2 -mmcu=$(MCU)
OBJCOPY = avr-objcopy

AVRDUDE = avrdude
PROGRAMMER = avrispmkII
PARTNO = x128a4u

C_SOURCES :=  $(wildcard drivers/*.c) \
			  $(wildcard freertos/*.c) \
			  $(wildcard freertos/portable/GCC/ATxmega128A4U/*.c) \
			  freertos/portable/MemMang/heap_1.c
CXX_SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.c,%.c.o,$(C_SOURCES)) $(patsubst %.cpp,%.cpp.o,$(CXX_SOURCES))

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

%.c.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

program: $(TARGET).hex
	$(AVRDUDE) -p $(PARTNO) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex

clean:
	rm -f $(OBJECTS) $(TARGET).elf $(TARGET).hex
