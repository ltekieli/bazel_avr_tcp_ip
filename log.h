#ifndef LOG_H_
#define LOG_H_

void log_uart_init();

void log_info_fmt(const char* fmt, ...);

void log_warning_fmt(const char* fmt, ...);

void log_error_fmt(const char* fmt, ...);

void log_info(const char* s);

void log_warning(const char* s);

void log_error(const char* s);

#ifdef ENABLE_LOGGING

#define LOG_INFO_FMT(fmt, ...) log_info_fmt(fmt, __VA_ARGS__)
#define LOG_WARNING_FMT(fmt, ...) log_warning_fmt(fmt, __VA_ARGS__)
#define LOG_ERROR_FMT(fmt, ...) log_error_fmt(fmt, __VA_ARGS__)

#define LOG_INFO(msg) log_info(msg)
#define LOG_WARNING(msg) log_warning(msg)
#define LOG_ERROR(msg) log_error(msg)

#else

#define LOG_NOOP do {} while (0)

#define LOG_INFO_FMT(fmt, ...) LOG_NOOP
#define LOG_WARNING_FMT(fmt, ...) LOG_NOOP
#define LOG_ERROR_FMT(fmt, ...) LOG_NOOP

#define LOG_INFO(msg) LOG_NOOP
#define LOG_WARNING(msg) LOG_NOOP
#define LOG_ERROR(msg) LOG_NOOP

#endif

#endif // LOG_H_
