#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../src/dynamic/buffer.h"
#include "../src/dynamic/stream.h"

void core()
{
  buffer b;
  stream s;

  buffer_construct(&b);
  stream_construct(&s, &b);
  stream_write8(&s, 42);
  stream_write16(&s, 4711);
  stream_write32(&s, 1048576);
  stream_write64(&s, 1000000000000);
  stream_write64(&s, stream_write_bits(42, 64, 20, 6));

  assert_int_equal(stream_size(&s), 1 + 2 + 4 + 8 + 8);
  assert_int_equal(stream_read8(&s), 42);
  assert_int_equal(stream_read16(&s), 4711);
  assert_int_equal(stream_read32(&s), 1048576);
  assert_int_equal(stream_read64(&s), 1000000000000);
  assert_int_equal(stream_read_bits(stream_read64(&s), 64, 20, 6), 42);

  // overrun invalidates stream
  assert_int_equal(stream_read8(&s), 0);
  assert_false(stream_valid(&s));
  assert_int_equal(stream_size(&s), 0);

  // calls on invalid stream
  stream_read(&s, NULL, 0);
  (void) stream_read8(&s);
  (void) stream_read16(&s);
  (void) stream_read32(&s);
  (void) stream_read64(&s);
  stream_write(&s, NULL, 0);
  (void) stream_write8(&s, 0);
  (void) stream_write16(&s, 0);
  (void) stream_write32(&s, 0);
  (void) stream_write64(&s, 0);
  assert_false(stream_valid(&s));

  stream_destruct(&s);
  buffer_destruct(&b);
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(core),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
