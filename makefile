CPPDEFINES=-DF_CPU=1000000

all:
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c controller.cpp -ocontroller.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c speed_leds.cpp -ospeed_leds.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c battery_leds.cpp -obattery_leds.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c switches.cpp -oswitches.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c battery.cpp -obattery.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c adc.cpp -oadc.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -O1 $(CPPDEFINES) -c util.cpp -outil.o
	avr-g++ -ocontroller.elf controller.o speed_leds.o battery_leds.o switches.o battery.o adc.o util.o -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega8535  
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures controller.elf controller.hex

upload:
	avrdude -c usbtiny -p atmega8535 -U flash:w:controller.hex:i
