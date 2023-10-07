#include "timer.h"
#include "uart.h"

#include <inttypes.h>
#include <stdarg.h>

static FILE uart_str = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

const char INF_PREFIX[] = "[%" PRIu32 "][INF]: ";
const char WRN_PREFIX[] = "[%" PRIu32 "][WRN]: ";
const char ERR_PREFIX[] = "[%" PRIu32 "][ERR]: ";

static void log_impl(const char* prefix, const char* fmt, va_list argp)
{
    printf(prefix, timer_get_system_tick());
    vprintf(fmt, argp);
    printf("\r\n");
}

void log_uart_init()
{
    stdout = &uart_str;
}

void log_info_fmt(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_impl(INF_PREFIX, fmt, args);
    va_end(args);
}

void log_warning_fmt(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_impl(WRN_PREFIX, fmt, args);
    va_end(args);
}

void log_error_fmt(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_impl(ERR_PREFIX, fmt, args);
    va_end(args);
}

void log_info(const char* s)
{
    log_info_fmt("%s", s);
}

void log_warning(const char* s)
{
    log_warning_fmt("%s", s);
}

void log_error(const char* s)
{
    log_error_fmt("%s", s);
}

