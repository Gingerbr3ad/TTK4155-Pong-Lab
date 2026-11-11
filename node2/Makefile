# Node 2 - ATSAM3X8E

F_CPU := 84000000UL
BAUD := 9600

SAM_DIR := utils/sam

MCUTYPE := __SAM3X8E__
LDSCRIPT := $(SAM_DIR)/flash.ld
OPENOCD_CFG := $(SAM_DIR)/openocd.cfg

BOOTUP := \
    $(SAM_DIR)/sam3x/source/exceptions.c \
    $(SAM_DIR)/sam3x/source/startup_sam3x.c \
    $(SAM_DIR)/sam3x/source/system_sam3x.c

APP_SOURCES := \
	src/application/servo.c \
	src/application/motor.c \
	src/application/motor_calibration.c \
	src/application/position_controller.c \
    src/main.c

DRIVER_SOURCES := \
    src/drivers/uart_driver.c \
    src/drivers/can_driver.c \
	src/drivers/pwm_driver.c \
	src/drivers/encoder_driver.c \
	src/drivers/adc_driver.c \
	src/drivers/solenoid_driver.c

UTILS_SOURCES := \
    utils/time.c \
	utils/system_tick_60Hz.c

SOURCE_FILES := $(BOOTUP) $(APP_SOURCES) $(DRIVER_SOURCES) $(UTILS_SOURCES)

# Feel free to ignore anything below this line
BUILD_DIR := build
ELF := $(BUILD_DIR)/main.elf
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SOURCE_FILES))

# Tools
CC:=arm-none-eabi-gcc
LD:=arm-none-eabi-gcc
AR:=arm-none-eabi-ar
AS:=arm-none-eabi-as

CFLAGS := -mcpu=cortex-m3 -mthumb -g -std=c11
CFLAGS += -MMD -MP
CFLAGS += -D$(MCUTYPE)

CFLAGS += -Iinclude
CFLAGS += -Iutils
CFLAGS += -I$(SAM_DIR)
CFLAGS += -I$(SAM_DIR)/sam3x/include
CFLAGS += -I$(SAM_DIR)/sam3x/source
CFLAGS += -I$(SAM_DIR)/cmsis

CPPFLAGS := -DF_CPU=$(F_CPU)
CPPFLAGS += -DBAUD=$(BAUD)

LDFLAGS := -T$(LDSCRIPT)
LDFLAGS += -mthumb -mcpu=cortex-m3
LDFLAGS += -Wl,--gc-sections

.DEFAULT_GOAL := $(ELF)
# compile and generate dependancy info
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	#$(CC) -MM $(CFLAGS) $< > $@.d

$(BUILD_DIR)/%.o : %.s | $(BUILD_DIR)
	$(AS) $< -o $@

$(ELF) : $(OBJS) $(LDSCRIPT) | $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) $(ELF)

.PHONY: debug
debug: $(ELF)
	if pgrep openocd; then killall -s 9 openocd; fi
	x-terminal-emulator -e openocd -f $(OPENOCD_CFG) -c "program $(ELF) verify"
	sleep 5
	arm-none-eabi-gdb -tui -iex "target extended-remote localhost:3333" $(ELF)
	killall -s 9 openocd

.PHONY: flash
flash: $(ELF)
	#./src/utils/sam/write_bin.sh src/utils/sam/openocd.cfg $(BUILD_DIR)/main.elf
	if pgrep openocd; then killall -s 9 openocd; fi
	openocd -f $(OPENOCD_CFG) -c "program ${BUILD_DIR}/main.elf verify reset exit"


# pull in dependencies
-include    $(OBJS:.o=.d)


