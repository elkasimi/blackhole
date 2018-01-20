#include "IO.h"

#include "Position.h"

#include <assert.h>
#include <stdarg.h>

int32_t io_read_move(const char* s) {
  char field[3];
  int32_t value;
  assert(sscanf(s, "%[^=]=%d", field, &value) == 2);

  return CREATE_MOVE(value, field_to_place(field));
}

int fprflush(FILE* const stream, const char* fmt, ...) {
  int r;
  va_list ap;
  va_start(ap, fmt);
  r = vfprintf(stream, fmt, ap);
  va_end(ap);
  fflush(stream);
  return r;
}

void io_display_move(int32_t move, FILE* out) {
  char s[10];
  sprintf(s, "%s=%d", place_to_field(MOVE_PLACE(move)), MOVE_VALUE(move));

  fprflush(out, "%s\n", s);
}
