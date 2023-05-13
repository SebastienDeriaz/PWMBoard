# AVR PWM software

## dnf install list

- ``avrdude``
- ``gcc-avr32-linux-gnu``
- ``binutils-avr32-linux-gnu``
- ``avr-libc``
- ``avr-binutils``
- ``avr-gcc``

Compiler : ``avr32-linux-gnu-gcc``

## Compilation

``avr-c++ -mmcu=attiny2313 -O2 main.cpp -o main.elf``

## Flash