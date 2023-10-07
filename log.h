#ifndef LOG_H_
#define LOG_H_

void log_uart_init();

void log_info_fmt(const char* fmt, ...);

void log_warning_fmt(const char* fmt, ...);

void log_error_fmt(const char* fmt, ...);

void log_info(const char* s);

void log_warning(const char* s);

void log_error(const char* s);

#endif // LOG_H_
