#define HC_STRING_IMPL
#include "../hc_string.h"

#define TEXT_WITH_REPETITION \
    "The sun was shining brightly. The sun warmed the earth, and the sun's rays made everything glow.\n" \
    "People were out enjoying the sun, basking in the sun.\n" \
    "Some even closed their eyes to feel the sun on their faces.\n" \
    "The sun felt like a gentle embrace, a reminder of the sun's power."

int main(void)
{
    hc_string_t txt1 = hc_string_create_from_cstr("Hello, World!");
    hc_string_substring(&txt1, 7, 5);

    hc_string_t txt2 = hc_string_create_from_cstr("Hello, ");
    hc_string_concat_hc(&txt2, &txt1);

    hc_string_append_char(&txt2, '!');

    hc_string_concat(&txt2, "\n" TEXT_WITH_REPETITION);
    hc_string_replace(&txt2, "sun", "rain");

    printf("%s\n", txt2.data);
    printf("Word count: %i\n", hc_string_word_count(&txt2));

    return 0;
}
