all:
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c adc.cpp -oadc.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c battery.cpp -obattery.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c battery_leds.cpp -obattery_leds.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c controller.cpp -ocontroller.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c debug.cpp -odebug.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c speed_leds.cpp -ospeed_leds.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c speed_sensor.cpp -ospeed_sensor.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c switches.cpp -oswitches.o
	avr-g++ -Wall -Wextra -mmcu=atmega8535 -DF_CPU=1000000 -O1 -c util.cpp -outil.o

	avr-g++ -oasap.elf adc.o battery.o battery_leds.o controller.o debug.o speed_leds.o speed_sensor.o switches.o util.o -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega8535  
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures asap.elf asap.hex

upload:
	avrdude -c usbtiny -p atmega8535 -U flash:w:asap.hex:i
