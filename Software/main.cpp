#include <avr/io.h>
#define F_CPU 8000000UL
#include <avr/iotn2313.h>
// #include <math.h>
// #include <stdint.h>
#include <util/delay.h>

class Pin {
    volatile uint8_t& _PORT;
    volatile uint8_t& _DDR;
    volatile uint8_t& _PIN;
    volatile uint8_t pin_number;
    void (*_enablePwm)(void) = nullptr;
    void (*_disablePwm)(void) = nullptr;
    void (*_setPwm)(uint16_t) = nullptr;
    bool pwmState = false;

   public:
    void setPwm(uint16_t value) {
        if (pwmState != (value != 0)) {
            if (value != 0) {
                pwmState = true;
                _enablePwm();
            } else {
                pwmState = false;
                _disablePwm();
            }
        }
        _setPwm(value);
    }

    Pin(volatile uint8_t& port, volatile uint8_t& ddr, volatile uint8_t& pin,
        uint8_t pin_number, void (*enablePwm)(void) = nullptr,
        void (*disablePwm)(void) = nullptr, void (*setPwm)(uint16_t) = nullptr)
        : _PORT(port),
          _DDR(ddr),
          _PIN(pin),
          pin_number(pin_number),
          _enablePwm(enablePwm),
          _disablePwm(disablePwm),
          _setPwm(setPwm){};

    enum Direction { INPUT, OUTPUT, INPUT_PULLUP };
    void setDirection(Direction direction) {
        switch (direction) {
            case INPUT:
                _DDR &= ~(1 << pin_number);
                _PORT &= ~(1 << pin_number);
                break;
            case OUTPUT:
                _DDR |= (1 << pin_number);
                _PORT &= ~(1 << pin_number);
                break;
            case INPUT_PULLUP:
                _DDR &= ~(1 << pin_number);
                _PORT |= (1 << pin_number);
                break;
        }
    }

    bool getState() { return (_PIN & (1 << pin_number)) != 0; }
    void setState(bool value) {
        _PORT = (_PORT & ~(1 << pin_number)) | ((value ? 1 : 0) << pin_number);
    }
};

class SoftHBridge {
    int32_t speed;
    const int32_t step = 1;
    Pin& _AL;
    Pin& _BL;
    Pin& _AH;
    Pin& _BH;
    int32_t ocrMax;

    public:
    void setSpeed(int32_t speed) {
        if (speed == 0) {
          _AH.setPwm(0);
          _BH.setPwm(0);
          _AL.setState(false);
          _BL.setState(false);
        }
        else if (speed > 0) {
            _AH.setPwm(speed);
            _BH.setPwm(0);
            _AL.setState(false);
            _BL.setState(true);
        } else {
            _AH.setPwm(0);
            _BH.setPwm(-speed);
            _AL.setState(true);
            _BL.setState(false);
        }
    }

   public:
    SoftHBridge(Pin& AL, Pin& BL, Pin& AH, Pin& BH, int32_t ocrMax)
        : _AL(AL), _BL(BL), _AH(AH), _BH(BH), ocrMax(ocrMax) {
        _AL.setDirection(Pin::OUTPUT);
        _BL.setDirection(Pin::OUTPUT);
        _AH.setDirection(Pin::OUTPUT);
        _BH.setDirection(Pin::OUTPUT);
        _AL.setState(false);
        _BL.setState(false);
        _AH.setPwm(0);
        _BH.setPwm(0);
        speed = 0;
    }

    void update(bool A, bool B) {

        if (A && speed >= 0) {
          speed += step;
          if (speed > ocrMax) {
            speed = ocrMax;
          }
        }
        else if (B && speed <= 0) {
          speed -= step;
          if (speed < -ocrMax) {
            speed = -ocrMax;
          }
        }
        else {
          // If it is close to zero, make it zero
            if ((speed < 0 ? -speed : speed) <= step) {
                speed = 0;
            } else if (speed > 0) {
                speed -= step;
            } else if (speed < 0) {
                speed += step;
            }
        }

        setSpeed(speed);
    }
};

int main(void) {
    /*   Configure Timer1   */
    // Set waveform generation mode to normal (TOP at 0xFFFF)
    TCCR1A = (0 << WGM10) | (1 << WGM11);
    // Set clock source to clk/1 (8MHz with 16 bits -> 122 Hz)
    TCCR1B =
        (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);

    ICR1 = 500;

    Pin BH(
        PORTB, DDRB, PINB, PB4,
        []() { TCCR1A |= (1 << COM1B1); },     // enable PWM
        []() { TCCR1A &= ~(1 << COM1B1); },    // disable PWM
        [](uint16_t value) { OCR1B = value; }  // set PWM
    );
    Pin AH(
        PORTB, DDRB, PINB, PB3,
        []() { TCCR1A |= (1 << COM1A1); },     // enable PWM
        []() { TCCR1A &= ~(1 << COM1A1); },    // disable PWM
        [](uint16_t value) { OCR1A = value; }  // set PWM
    );
    Pin BL(PORTB, DDRB, PINB, PB2);
    Pin AL(PORTB, DDRB, PINB, PB1);

    Pin A(PORTA, DDRA, PINA, PA1);
    Pin B(PORTA, DDRA, PINA, PA0);

    A.setDirection(Pin::INPUT_PULLUP);
    B.setDirection(Pin::INPUT_PULLUP);

    SoftHBridge softHBridge(AL, BL, AH, BH, (int32_t)ICR1);


    while (1) {
        softHBridge.update(
          A.getState(),
          B.getState()
        );
        _delay_us(5000);
    }
}