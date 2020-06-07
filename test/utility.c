#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic.h"

void basic(__attribute__ ((unused)) void **state)
{
  char s[16];

  (void) utility_tsc();

  assert_true(segment_equal(segment_string("1000"), utility_u32_segment(1000)));
  assert_true(segment_equal(segment_string("999"), utility_u32_segment(999)));
  assert_true(segment_equal(segment_string("101"), utility_u32_segment(101)));

  utility_u32_toa(100, s);
  assert_string_equal(s, "100");
}

int main()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(basic)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
