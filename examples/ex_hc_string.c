#include "../hc_string.h"

int main(void)
{
    hc_string_t txt1 = hc_create_string("Hello, World!");
    hc_substring_string(&txt1, 7, 5);

    hc_string_t txt2 = hc_create_string("Hello, ");
    hc_concat_string_hc(&txt2, &txt1);

    hc_append_char_string(&txt2, '!');

    printf("%s\n", txt2.data);

    return 0;
}
