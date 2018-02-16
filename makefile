FCPU=8000000UL
MMCU=atmega8535
FLAGS=-Wall -Wextra -Os -fdata-sections -ffunction-sections

OBJ=adc.o \
battery.o \
battery_leds.o \
debug.o \
speed_leds.o \
speed_sensor.o \
switches.o \
util.o \
debug_led.o \
power_control.o \
serial.o \
charge_fader.o \
controller.o

%.o: %.cpp %.h
	avr-g++ $(FLAGS) -mmcu=$(MMCU) -DF_CPU=$(FCPU) $(OPTIMIZE) -c $< -o $@

all: $(OBJ)

	avr-g++ -oasap.elf $^ -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=$(MMCU)  
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures asap.elf asap.hex
	avr-size -C --mcu=$(MMCU) asap.elf

profile:
	avr-nm --size-sort asap.elf

upload:
	avrdude -c usbtiny -p $(MMCU) -U flash:w:asap.hex:i

upload-fuses:
	avrdude -c usbtiny -p $(MMCU) -U lfuse:w:0xe4:m -U hfuse:w:0xd9:m

clean:
	rm -rf *.o
	rm -rf *.map