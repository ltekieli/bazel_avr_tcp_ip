#include <avr/interrupt.h>
#include <avr/io.h>

static volatile uint32_t system_tick = 0;

ISR(TIMER1_COMPA_vect)
{
    ++system_tick;
}

void timer_init()
{
    // https://www.arduinoslovakia.eu/application/timer-calculator

    // Clear registers
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // 1000 Hz (16000000/((249+1)*64))
    OCR1A = 249;
    // CTC
    TCCR1B |= (1 << WGM12);
    // Prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // Output Compare Match A Interrupt Enable
    TIMSK1 |= (1 << OCIE1A);
}

uint32_t timer_get_system_tick()
{
    return system_tick;
}
