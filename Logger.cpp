//
// Created by gjk on 12/10/19.
//
#include <stdio.h>
#include <stdarg.h>
#include "Logger.h"

void Logger::INFO(const char *format_, ...) {
    va_list ap;
    va_start(ap,format_);
    vfprintf(stdout, format_,ap);
    va_end(ap);
}

void Logger::ERROR(const char *format_, ...) {
    va_list ap;
    va_start(ap,format_);
    vfprintf(stderr, format_,ap);
    va_end(ap);
}