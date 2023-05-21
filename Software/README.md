# AVR PWM software

## dnf install list

- ``avrdude``
- ``gcc-avr32-linux-gnu``
- ``binutils-avr32-linux-gnu``
- ``avr-libc``
- ``avr-binutils``
- ``avr-gcc``

C++ :

- ``avr-gcc-c++`` (compiler)
- ``avrdude`` (flash)
- ``avr-libc`` (libraries)

Compiler : ``avr32-linux-gnu-gcc`` or ``avr32-linux-gnu-gcc``

## Compilation

``avr-c++ -mmcu=attiny2313 -O2 main.cpp -o main.elf``

## Flash

``avrdude -p t2313 -c usbasp -U flash:w:main.elf``

## Notes

Libraries in ``/usr/avr/include``