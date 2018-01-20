#pragma once

#include <stdint.h>
#include <stdio.h>

int fprflush(FILE* const stream, const char* fmt, ...);

int32_t io_read_move(const char* s);

void io_display_move(int32_t move, FILE* out);
