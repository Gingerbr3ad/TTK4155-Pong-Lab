APP_SOURCES := \
    src/main.c \
    src/application/sram_test.c \
	src/application/joystick.c \
	src/application/oled/oled_utils.c

DRIVER_SOURCES := \
    src/drivers/adc_driver.c \
    src/drivers/external_memory_driver.c \
    src/drivers/uart_driver.c \
	src/drivers/spi_driver.c

SOURCE_FILES := $(APP_SOURCES) $(DRIVER_SOURCES)

# Set this flag to "yes" (no quotes) to use JTAG; otherwise ISP (SPI) is used
PROGRAM_WITH_JTAG := yes

PROGRAMMER := atmelice_isp
ifeq ($(PROGRAM_WITH_JTAG), yes)
	PROGRAMMER := atmelice
endif

BUILD_DIR := build
TARGET_CPU := atmega162
TARGET_DEVICE := m162

CC := avr-gcc # Compiler choice

# Compiler flags
CFLAGS := -O -std=c11 -ggdb
CFLAGS += -Wall -Wextra # Adds extra warnings during compilation, usefull for catching stupid errors
CFLAGS += -mmcu=$(TARGET_CPU)

# C Preprocessor flags
CPPFLAGS := -Iinclude # Sets the inc directory as the path for all includes
CPPFLAGS += -DF_CPU=4915200UL # The CPU clock speed in Hz, for us this is 4.9152 MHz since that is the frequency of the osicialtor we use as the clock
CPPFLAGS += -DBAUD=9600 # Defines the baud rate used by the AVR Libc macros, this works well here so long we only ever want to use a single baud value for everything, if we ever want two different baud rates this should be move to an include file

# Linker flags
LDFLAGS := -mmcu=$(TARGET_CPU)

OBJECT_FILES = $(SOURCE_FILES:%.c=$(BUILD_DIR)/%.o)

.DEFAULT_GOAL := $(BUILD_DIR)/main.hex

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/main.hex: $(OBJECT_FILES) | $(BUILD_DIR)
	$(CC) $(LDFLAGS) $(OBJECT_FILES) -o $(BUILD_DIR)/a.out
	avr-objcopy -j .text -j .data -O ihex $(BUILD_DIR)/a.out $(BUILD_DIR)/main.hex

.PHONY: flash
flash: $(BUILD_DIR)/main.hex
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: erase
erase:
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -e
	
.PHONY: debug
debug:
	if pgrep avarice; then pkill avarice; fi
	avrdude -p $(TARGET_DEVICE) -c $(PROGRAMMER) -U flash:w:$(BUILD_DIR)/main.hex:i
	x-terminal-emulator -e avarice --edbg --ignore-intr :4242
	sleep 2
	avr-gdb -tui -iex "target remote localhost:4242" $(BUILD_DIR)/a.out
	killall -s 9 avarice	