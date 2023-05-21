./compile.sh

avrdude -p t2313 -c usbasp -U flash:w:main.elf