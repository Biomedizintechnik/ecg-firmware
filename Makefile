MCU = atxmega128a4u
FORMAT = ihex
TARGET = ecg

CC = avr-g++
CXX = avr-g++
CFLAGS = -O2 -mmcu=$(MCU) -Iinclude -DF_CPU=32000000
CXXFLAGS = -O2 -mmcu=$(MCU) -Iinclude -DF_CPU=32000000
LIBS =
LD = avr-g++
LDFLAGS = -O2 -mmcu=$(MCU)
OBJCOPY = avr-objcopy

AVRDUDE = avrdude
PROGRAMMER = avrispmkII
PARTNO = x128a4u

C_SOURCES := $(wildcard src/*.c)
CXX_SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst %.c,%.o,$(C_SOURCES)) $(patsubst %.cpp,%.o,$(CXX_SOURCES))

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CXX) $(CXXFLAGS) -o $@ -c $<

program: $(TARGET).hex
	$(AVRDUDE) -p $(PARTNO) -c $(PROGRAMMER) -U flash:w:$(TARGET).hex

clean:
	rm -f $(OBJECTS) $(TARGET).elf $(TARGET).hex
