#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dynamic.h"

void basic(__attribute__((unused)) void **state)
{
  segment s1, s2;

  s1 = segment_string("test");
  s2 = segment_data("atest", 5);

  assert_false(segment_equal(s1, segment_string("nope")));
  assert_true(segment_equal(s1, segment_offset(s2, 1)));
  assert_false(segment_equal(s1, segment_empty()));
  assert_true(segment_equal_case(s1, segment_string("TeSt")));
  assert_false(segment_equal_case(s1, segment_string("TeStA")));
  assert_false(segment_equal_case(s1, segment_string("TeSA")));
}

int main()
{
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(basic)};

  return cmocka_run_group_tests(tests, NULL, NULL);
}
