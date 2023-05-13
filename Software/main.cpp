#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>


int main(void)
{
  DDRA = 0x03;

  while (1) {
    PORTA = 0x00;
    _delay_ms(5);
    PORTA = 0x03;
    _delay_ms(5);
  }
}