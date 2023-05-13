avr-c++ -mmcu=attiny2313 -O2 main.cpp -o main.elf

avrdude -p t2313 -c usbasp -U flash:w:main.elf