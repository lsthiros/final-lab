F_CPU = 16000000

default:
	avr-gcc -std=gnu99 -mmcu=atmega328p -DF_CPU=$(F_CPU) -O2 -o lab4_part1.elf lab4_part1.c 
	avr-objcopy -O ihex lab4_part1.elf lab4_part1.hex
	avr-size lab4_part1.elf

clean:
	rm -rf *.elf *.o *.hex

download_mac:
	#may need to change /dev/tty to correct port on your computer
	avrdude -p m328p -P /dev/tty.usbmodem1411 -c arduino -F -u -U flash:w:lab4_part1.hex

download_linux:
	#may need to change /dev/tty to correct port on your computer
	avrdude -p m328p -P /dev/ttyACM0 -c arduino -F -u -U flash:w:lab4_part1.hex

download_windows:
	avrdude -p m328p -P com3 -c arduino -F -u -U flash:w:lab4_part1.hex 
