#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

void timer_init();

uint32_t timer_get_system_tick();

#endif // TIMER_H_
