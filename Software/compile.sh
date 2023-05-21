#avr-c++ -mmcu=attiny2313 -O3 main.cpp -o main.elf -Wlstrip-all

avr-g++ -c -g -Os -w -mmcu=attiny2313 main.cpp -o main.o
avr-strip --strip-debug main.o --strip-unneeded
avr-g++ -g -Os -w -mmcu=attiny2313 -Wl,--gc-sections main.o -o main.elf

#avr-c++ -mmcu=attiny2313 -Os main.cpp -o main.elf