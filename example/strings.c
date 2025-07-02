#include <stdio.h>
#include <dynamic.h>

int main()
{
  char storage[4], chars[] = "abc\U00000000\u00e5\u0250\u00e4\u03B6\ufffd\u2103\u25A2\u2780\U0001F600";
  string_t c, s = data(chars, sizeof chars - 1), emojis;
  size_t i;

  /* create a unicode string of emojis */
  emojis = string_null();
  printf("[emojis] ");
  for (i = 0x0001f600; i < 0x0001f620; i++)
    emojis = string_append(emojis, string_character(data(storage, sizeof storage), i));
  string_fwrite(emojis, stdout);
  printf("\n");
  string_free(emojis);

  /* analyse unicode string containing the zero codepoint */
  printf("[string] \"");
  string_fwrite(s, stdout);
  printf("\"\n");
  printf("[valid] %s\n", string_valid(s) ? "true" : "false");
  printf("[size] %lu\n", string_size(s));
  printf("[length] %lu\n", string_length(s));
  i = 0;
  string_foreach(s, c)
    printf("[char %lu, size %lu, codepoint 0x%08x] '%.*s'\n", i++, string_size(c), string_codepoint(c), (int) string_size(c), string_base(c));
}
